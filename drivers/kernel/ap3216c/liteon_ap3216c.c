/*
 * ap3216c_driver.c
 *
 * AP3216C ALS / Proximity / IR sensor driver
 * Target: NXP i.MX6ULL, Linux 4.1.15
 *
 * Device Tree:
 * &i2c1 {
 *     status = "okay";
 *     ap3216c@1e {
 *         compatible = "liteon,ap3216c";
 *         reg = <0x1e>;
 *         status = "okay";
 *     };
 * };
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "../../../include/uapi/liteon_ap3216c.h"

/* AP3216C Register Definition */
#define AP3216C_REG_SYS_CFG     0x00
#define AP3216C_REG_IR_LOW      0x0A
#define AP3216C_REG_IR_HIGH     0x0B
#define AP3216C_REG_ALS_LOW     0x0C
#define AP3216C_REG_ALS_HIGH    0x0D
#define AP3216C_REG_PS_LOW      0x0E
#define AP3216C_REG_PS_HIGH     0x0F

/* System mode */
#define AP3216C_MODE_POWER_DOWN 0x00
#define AP3216C_MODE_ALS        0x01
#define AP3216C_MODE_PS_IR      0x02
#define AP3216C_MODE_ALS_PS_IR  0x03
#define AP3216C_MODE_SW_RESET   0x04

struct ap3216c_device {
    struct i2c_client *client;
    struct miscdevice miscdev;  /* 产生 /dev/ap3216c */
    struct mutex lock;          /* 防止多线程并发访问 I2C 造成数据错乱 */
};

/* ============================================================
 * AP3216C register access
 * ============================================================ */

static int ap3216c_read_reg(struct ap3216c_device *ap, u8 reg)
{
    s32 ret = i2c_smbus_read_byte_data(ap->client, reg);
    if (ret < 0)
        dev_err(&ap->client->dev, "failed to read reg 0x%02x: %d\n", reg, ret);
    return ret;
}

static int ap3216c_write_reg(struct ap3216c_device *ap, u8 reg, u8 value)
{
    s32 ret = i2c_smbus_write_byte_data(ap->client, reg, value);
    if (ret < 0)
        dev_err(&ap->client->dev, "failed to write reg 0x%02x: %d\n", reg, ret);
    return ret;
}

/* ============================================================
 * Hardware initialization
 * ============================================================ */

static int ap3216c_hw_init(struct ap3216c_device *ap)
{
    int ret;

    /* 1. Software reset */
    ret = ap3216c_write_reg(ap, AP3216C_REG_SYS_CFG, AP3216C_MODE_SW_RESET);
    if (ret)
        return ret;

    /* Datasheet 要求 reset 后至少等待 10ms，probe 上下文允许 msleep */
    msleep(20);

    /* 2. Enable ALS + PS + IR continuous mode */
    ret = ap3216c_write_reg(ap, AP3216C_REG_SYS_CFG, AP3216C_MODE_ALS_PS_IR);
    if (ret)
        return ret;

    /* 首次完整转换需要时间，等待第一组有效数据生成 */
    msleep(120);

    return 0;
}

/* ============================================================
 * AP3216C 的寄存器是连续排列的，可以改成一次读取 6 个字节：
 * ============================================================ */
static int ap3216c_get_sample(struct ap3216c_device *ap,
                              struct ap3216c_sample *sample)
{
    u8 data[6];
    int ret;
    memset(sample, 0, sizeof(*sample));
    ret = i2c_smbus_read_i2c_block_data(ap->client,AP3216C_REG_IR_LOW,sizeof(data),data);
    if (ret < 0)
        return ret;
    if (ret != sizeof(data))
        return -EIO;
    sample->ir = ((u16)data[1] << 2) | (data[0] & 0x03);
    sample->ir_valid = !(data[0] & BIT(7));
    sample->als = ((u16)data[3] << 8) | data[2];
    sample->ps = ((u16)(data[5] & 0x3F) << 4)| (data[4] & 0x0F);
    sample->ps_valid = !(data[4] & BIT(6));
    sample->object_near = !!(data[4] & BIT(7));
    return 0;
}


/* ============================================================
 * Character device interface
 * ============================================================ */

static int ap3216c_open(struct inode *inode, struct file *filp)
{
    /* miscdevice 框架调用 open 时，private_data 已指向 miscdevice 结构 */
    struct miscdevice *misc = filp->private_data;
    struct ap3216c_device *ap = container_of(misc, struct ap3216c_device, miscdev);

    filp->private_data = ap;
    return 0;
}

static int ap3216c_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/* ioctl 接口，返回 AP3216C 的完整状态或单独的 IR/ALS/PS 数据
 * 注意：必须先读 LOW 再读 HIGH，因为读取 LOW 时当前的 HIGH 值会被锁存
 *一次 ioctl 只读取一次完整传感器数据，并且不会在用户态拷贝期间占用锁。
 */
static long ap3216c_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
    struct ap3216c_device *ap = filp->private_data;
    struct ap3216c_sample sample;
    void __user *argp = (void __user *)arg;
    u16 value = 0;
    int ret;
    if (_IOC_TYPE(cmd) != AP3216C_IOC_MAGIC)
        return -ENOTTY;
    ret = mutex_lock_interruptible(&ap->lock);
    if (ret)
        return ret;
    ret = ap3216c_get_sample(ap, &sample);
    mutex_unlock(&ap->lock);
    if (ret)
        return ret;
    switch (cmd) {
    case AP3216C_GET_SAMPLE:
        if (copy_to_user(argp, &sample, sizeof(sample)))
            return -EFAULT;
        return 0;
    case AP3216C_GET_PS:
        value = sample.ps;
        break;
    case AP3216C_GET_ALS:
        value = sample.als;
        break;
    case AP3216C_GET_IR:
        value = sample.ir;
        break;
    default:
        return -ENOTTY;
    }
    if (copy_to_user(argp, &value, sizeof(value)))
        return -EFAULT;

    return 0;
}

static const struct file_operations ap3216c_fops = {
    .owner          = THIS_MODULE,
    .open           = ap3216c_open,
    .release        = ap3216c_release,
    .unlocked_ioctl = ap3216c_ioctl,
    .llseek         = no_llseek,
};

/* ============================================================
 * I2C Driver Framework
 * ============================================================ */

static int ap3216c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct ap3216c_device *ap;
    int ret;

    dev_info(&client->dev, "AP3216C probe start, addr=0x%02x\n", client->addr);

    /* 1. Check adapter functionality */
    if (!i2c_check_functionality(client->adapter,I2C_FUNC_SMBUS_I2C_BLOCK)) {
        dev_err(&client->dev,"I2C adapter does not support SMBus I2C block\n");
        return -EOPNOTSUPP;
    }

    /* 2. Allocate device private data (auto-freed on remove) */
    ap = devm_kzalloc(&client->dev, sizeof(*ap), GFP_KERNEL);
    if (!ap)
        return -ENOMEM;

    ap->client = client;
    mutex_init(&ap->lock);
    i2c_set_clientdata(client, ap);

    /* 3. Initialize AP3216C hardware */
    ret = ap3216c_hw_init(ap);
    if (ret) {
        dev_err(&client->dev, "failed to initialize AP3216C: %d\n", ret);
        return ret;
    }

    /* 4. Register misc character device */
    ap->miscdev.minor = MISC_DYNAMIC_MINOR;
    ap->miscdev.name = "ap3216c";
    ap->miscdev.fops = &ap3216c_fops;
    ap->miscdev.parent = &client->dev;

    ret = misc_register(&ap->miscdev);
    if (ret) {
        dev_err(&client->dev, "failed to register misc device: %d\n", ret);
        /* 注册失败时进入低功耗模式关闭传感器 */
        ap3216c_write_reg(ap, AP3216C_REG_SYS_CFG, AP3216C_MODE_POWER_DOWN);
        return ret;
    }

    dev_info(&client->dev, "AP3216C initialized successfully (/dev/ap3216c)\n");
    return 0;
}

static int ap3216c_remove(struct i2c_client *client)
{
    struct ap3216c_device *ap = i2c_get_clientdata(client);

    /* 注销字符设备，阻止新的 open 与 ioctl 调用 */
    misc_deregister(&ap->miscdev);

    /* 进入 Power Down 模式 */
    mutex_lock(&ap->lock);
    ap3216c_write_reg(ap, AP3216C_REG_SYS_CFG, AP3216C_MODE_POWER_DOWN);
    mutex_unlock(&ap->lock);

    dev_info(&client->dev, "AP3216C driver removed\n");
    return 0;
}

static const struct i2c_device_id ap3216c_id[] = {
    { "ap3216c", 0 },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, ap3216c_id);

static const struct of_device_id ap3216c_of_match[] = {
    { .compatible = "liteon,ap3216c" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ap3216c_of_match);

static struct i2c_driver ap3216c_driver = {
    .driver = {
        .name = "ap3216c",
        .owner = THIS_MODULE,
        .of_match_table = ap3216c_of_match,
    },
    .probe = ap3216c_probe,
    .remove = ap3216c_remove,
    .id_table = ap3216c_id,
};

/* 自动生成 module_init 和 module_exit */
module_i2c_driver(ap3216c_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("JJL");
MODULE_DESCRIPTION("AP3216C ALS/PS/IR driver for i.MX6ULL");