/*
 * smarthome_beep.c
 *
 * 用户态可以这样控制:echo 1 > /dev/beep_device开启蜂鸣器
 * echo 0 > /dev/beep_device关闭蜂鸣器   
 * 读取当前逻辑状态： cat /dev/beep_device
 * 
 * i.MX6ULL GPIO Buzzer Driver
 *
 * Device Tree:
 * smarthome_beep: smarthome-beep {
 *     compatible = "smarthome,beep";
 *     pinctrl-names = "default";
 *     pinctrl-0 = <&pinctrl_beep>;
 *     beep-gpios = <&gpio5 1 GPIO_ACTIVE_HIGH>;
 *     status = "okay";
 * };
 */



#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DRIVER_NAME     "smarthome_beep"
#define DEVICE_NAME     "beep_device"
#define CLASS_NAME      "beep_class"

/*
 * 一个蜂鸣器设备对应一个 beep_device 实例
 */
struct beep_device {
    dev_t deviceid;
    struct cdev cdev;
    struct device *device;
    /* 对应设备树 beep-gpios = <&gpio5 1 GPIO_ACTIVE_HIGH>; */
    struct gpio_desc *beep_gpio;
    struct mutex lock; /* 保护设备蜂鸣器 GPIO 的并发访问*/\
    int state;
};

/* class 属于整个驱动，而不是某一个硬件实例 */
static struct class *beep_class;

static int beep_device_open(struct inode *inode, struct file *filp)
{
    struct beep_device *beep;

    /* 通过 inode 的 cdev 找到包含它的 beep_device 结构体 */
    beep = container_of(inode->i_cdev, struct beep_device, cdev);

    /* 保存到 file->private_data，后续 read/write 可直接使用 */
    filp->private_data = beep;

    return 0;
}

static int beep_device_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t beep_device_read(struct file *filp, char __user *buf,
                                size_t size, loff_t *offset)
{
    struct beep_device *beep = filp->private_data;
    char status_buf[4];
    int value;
    int len;
    int ret;

    ret = mutex_lock_interruptible(&beep->lock);
    if (ret)
        return ret;

    value = beep->state;

    len = scnprintf(status_buf, sizeof(status_buf), "%d\n",
                    value ? 1 : 0);

    ret = simple_read_from_buffer(buf, size, offset,
                                status_buf, len);

out_unlock:
    mutex_unlock(&beep->lock);
    return ret;
}

static ssize_t beep_device_write(struct file *filp,const char __user *buf,
                                size_t size,loff_t *offset)
{
    struct beep_device *beep = filp->private_data;
    int value;
    int ret;
    ret = mutex_lock_interruptible(&beep->lock);
    if (ret)
        return ret;
    ret = kstrtoint_from_user(buf, size, 0, &value);
    if (ret)
        goto out_unlock;

    if (value != 0 && value != 1) {
        ret = -EINVAL;
        goto out_unlock;
    }
    gpiod_set_value_cansleep(beep->beep_gpio, value);
    beep->state = value;
    dev_info(beep->device,
        "write logical value=%d, gpio readback=%d\n",
        value,
        gpiod_get_value_cansleep(beep->beep_gpio));
    ret = size;
out_unlock:
    mutex_unlock(&beep->lock);
    return ret;
}

static const struct file_operations beep_device_ops = {
    .owner   = THIS_MODULE,
    .open    = beep_device_open,
    .release = beep_device_release,
    .read    = beep_device_read,
    .write   = beep_device_write,
    .llseek  = no_llseek,
};

static int beep_driver_probe(struct platform_device *pdev)
{
    struct beep_device *beep;
    int ret;

    dev_info(&pdev->dev, "smarthome beep probe start\n");

    /* 1. 分配设备私有数据 (Device Managed 自动释放) */
    beep = devm_kzalloc(&pdev->dev, sizeof(*beep), GFP_KERNEL);
    if (!beep)
        return -ENOMEM;

    mutex_init(&beep->lock);

    /* 2. 获取 GPIO 描述符并默认输出低电平关闭蜂鸣器 */
    beep->beep_gpio = devm_gpiod_get(&pdev->dev, "beep", GPIOD_OUT_LOW);
    beep->state = 0; /* 默认关闭蜂鸣器 */
    if (IS_ERR(beep->beep_gpio)) {
        ret = PTR_ERR(beep->beep_gpio);
        dev_err(&pdev->dev, "failed to get beep gpio: %d\n", ret);
        return ret;
    }

    /* 3. 动态申请设备号 */
    ret = alloc_chrdev_region(&beep->deviceid, 0, 1, DEVICE_NAME);
    if (ret) {
        dev_err(&pdev->dev, "alloc_chrdev_region failed: %d\n", ret);
        return ret;
    }

    /* 4. 初始化字符设备 */
    cdev_init(&beep->cdev, &beep_device_ops);
    beep->cdev.owner = THIS_MODULE;

    /* 5. 将字符设备注册到内核 */
    ret = cdev_add(&beep->cdev, beep->deviceid, 1);
    if (ret) {
        dev_err(&pdev->dev, "cdev_add failed: %d\n", ret);
        goto err_unregister_chrdev;
    }

    /* 6. 在 sysfs 中创建设备节点 (/dev/beep_device) */
    beep->device = device_create(beep_class, &pdev->dev, beep->deviceid,
                                 beep, DEVICE_NAME);
    if (IS_ERR(beep->device)) {
        ret = PTR_ERR(beep->device);
        dev_err(&pdev->dev, "device_create failed: %d\n", ret);
        goto err_cdev_del;
    }

    /* 7. 绑定私有数据到 pdev，方便 remove 函数取回 */
    platform_set_drvdata(pdev, beep);

    dev_info(&pdev->dev, "smarthome beep driver probed successfully (/dev/%s)\n",
             DEVICE_NAME);

    return 0;

err_cdev_del:
    cdev_del(&beep->cdev);
err_unregister_chrdev:
    unregister_chrdev_region(beep->deviceid, 1);
    return ret;
}

static int beep_driver_remove(struct platform_device *pdev)
{
    struct beep_device *beep = platform_get_drvdata(pdev);

    /* 退出前确保蜂鸣器处于关闭状态 */
    mutex_lock(&beep->lock);
    gpiod_set_value_cansleep(beep->beep_gpio, 0);
    beep->state = 0;
    mutex_unlock(&beep->lock);

    /* 倒序清理未被 devm_ 框架接管的资源 */
    device_destroy(beep_class, beep->deviceid);
    cdev_del(&beep->cdev);
    unregister_chrdev_region(beep->deviceid, 1);

    dev_info(&pdev->dev, "smarthome beep driver removed\n");
    return 0;
}

/* Device Tree 匹配表 */
static const struct of_device_id beep_of_match[] = {
    { .compatible = "smarthome,beep" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, beep_of_match);

static struct platform_driver beep_driver = {
    .probe  = beep_driver_probe,
    .remove = beep_driver_remove,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = beep_of_match,
    },
};

static int __init beep_driver_init(void)
{
    int ret;

    /* 优先创建全局 class */
    beep_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(beep_class)) {
        ret = PTR_ERR(beep_class);
        pr_err("beep: class_create failed: %d\n", ret);
        return ret;
    }

    ret = platform_driver_register(&beep_driver);
    if (ret) {
        pr_err("beep: platform_driver_register failed: %d\n", ret);
        class_destroy(beep_class);
        return ret;
    }

    pr_info("smarthome beep driver loaded\n");
    return 0;
}

static void __exit beep_driver_exit(void)
{
    /* 清理顺序：先卸载 driver，再销毁 class */
    platform_driver_unregister(&beep_driver);
    class_destroy(beep_class);
    pr_info("smarthome beep driver unloaded\n");
}

module_init(beep_driver_init);
module_exit(beep_driver_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("JJL");
MODULE_DESCRIPTION("i.MX6ULL SmartHome GPIO Buzzer Driver");