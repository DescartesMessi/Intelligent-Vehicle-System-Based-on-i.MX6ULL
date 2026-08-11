/*
 * smarthome_sr04.c
 * HC-SR04 / SR04 GPIO Ultrasonic Sensor Driver
 * Linux 4.1.15
 * Device Tree:
 * sr04: sr04 {
 *     compatible = "smarthome,sr04";
 *     pinctrl-names = "default";
 *     pinctrl-0 = <&pinctrl_sr04>;
 *     trig-gpios = <&gpio4 19 GPIO_ACTIVE_HIGH>;
 *     echo-gpios = <&gpio4 20 GPIO_ACTIVE_HIGH>;
 *     status = "okay";
 * };
 */

/*
* 编译和安装：
* cd /home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/sr04
* make clean --> make --> make install 
* 确认模块 --> ls -lh /home/pointer/imx6ull/projects/Vehicle-system/drivers/kernel/sr04/smarthome_sr04.ko
* ls -lh /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/deploy/nfs/rootfs/lib/modules/4.1.15+/extra/
*/

#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/ktime.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/jiffies.h>
#include "../../../include/uapi/smarthome_sr04.h"
#define DRIVER_NAME             "smarthome_sr04"
#define DEVICE_NAME             "sr04"

#define SR04_TRIGGER_US         12
#define SR04_TIMEOUT_MS         50
#define SR04_MIN_PULSE_US       100
#define SR04_MAX_PULSE_US       30000

struct sr04_device {
    struct device *dev;
    struct gpio_desc *trig_gpio;
    struct gpio_desc *echo_gpio;
    int irq;
    struct miscdevice miscdev;
    struct mutex measure_lock;
    struct completion measurement_done;
    spinlock_t state_lock;
    bool waiting_echo;
    bool rise_seen;
    bool measurement_valid;
    ktime_t rise_time;
    u32 pulse_us;
};

/*
 * Echo 中断处理函数
 *
 * Echo 上升沿：记录开始时间
 * Echo 下降沿：计算高电平持续时间
 */
static irqreturn_t sr04_irq_handler(int irq, void *data)
{
    struct sr04_device *sr04 = data;
    unsigned long flags;
    int level;
    bool complete_measurement = false;
    ktime_t now;
    s64 pulse_us;
    /*
     * GPIO 控制器为 i.MX6ULL 本地 GPIO，
     * 中断上下文中使用非 cansleep 版本。
     */
    level = gpiod_get_value(sr04->echo_gpio);
    if (level < 0)
        return IRQ_HANDLED;
    now = ktime_get();
    spin_lock_irqsave(&sr04->state_lock, flags);
    if (!sr04->waiting_echo) {
        spin_unlock_irqrestore(&sr04->state_lock, flags);
        return IRQ_HANDLED;
    }

    if (level) {
        /*
         * Echo 上升沿
         */
        sr04->rise_time = now;
        sr04->rise_seen = true;
    } else if (sr04->rise_seen) {
        /*
         * Echo 下降沿
         */
        pulse_us = ktime_to_us(ktime_sub(now, sr04->rise_time));
        if (pulse_us >= SR04_MIN_PULSE_US &&
            pulse_us <= SR04_MAX_PULSE_US) {
            sr04->pulse_us = (u32)pulse_us;
            sr04->measurement_valid = true;
            complete_measurement = true;
        }
        sr04->waiting_echo = false;
        sr04->rise_seen = false;
    }
    spin_unlock_irqrestore(&sr04->state_lock, flags);
    if (complete_measurement)
        complete(&sr04->measurement_done);
    return IRQ_HANDLED;
}

/*
 * 执行一次完整测距
 *
 * 调用者必须已经持有 measure_lock。
 */static int sr04_do_measurement(struct sr04_device *sr04,struct sr04_measurement *result)
{
    unsigned long flags;
    long wait_ret;
    u32 pulse_us;
    bool valid;

    memset(result, 0, sizeof(*result));
    /*先确保 TRIG 处于低电平*/
    gpiod_set_value_cansleep(sr04->trig_gpio, 0);
    udelay(2);
    /*清除上一次 completion 状态，防止旧中断影响本次测量。*/
    reinit_completion(&sr04->measurement_done);

    /*初始化本次测量状态。*/
    spin_lock_irqsave(&sr04->state_lock, flags);
    sr04->waiting_echo = true;
    sr04->rise_seen = false;
    sr04->measurement_valid = false;
    sr04->pulse_us = 0;
    spin_unlock_irqrestore(&sr04->state_lock, flags);

    /*发送 SR04 触发脉冲。高电平时间必须大于 10us。*/
    gpiod_set_value_cansleep(sr04->trig_gpio, 1);
    udelay(SR04_TRIGGER_US);
    gpiod_set_value_cansleep(sr04->trig_gpio, 0);

    /*最多等待 50ms。*/
    wait_ret = wait_for_completion_interruptible_timeout(
        &sr04->measurement_done,
        msecs_to_jiffies(SR04_TIMEOUT_MS));

    /*等待被信号打断。*/
    if (wait_ret < 0) {
        spin_lock_irqsave(&sr04->state_lock, flags);
        sr04->waiting_echo = false;
        sr04->rise_seen = false;
        sr04->measurement_valid = false;
        spin_unlock_irqrestore(&sr04->state_lock, flags);
        return -ERESTARTSYS;
    }

    /*超时，通常表示：1. Echo 没有连接；2. Echo 没有产生下降沿；3. GPIO 或中断配置错误。*/
    if (wait_ret == 0) {
        spin_lock_irqsave(&sr04->state_lock, flags);
        sr04->waiting_echo = false;
        sr04->rise_seen = false;
        sr04->measurement_valid = false;
        spin_unlock_irqrestore(&sr04->state_lock, flags);
        result->valid = 0;
        return -ETIMEDOUT;
    }

    /*读取中断中保存的 Echo 脉宽。*/
    spin_lock_irqsave(&sr04->state_lock, flags);
    pulse_us = sr04->pulse_us;
    valid = sr04->measurement_valid;
    spin_unlock_irqrestore(&sr04->state_lock, flags);
    if (!valid)
        return -EIO;

    /*声速约为 343m/s。超声波经过的是往返距离，因此：distance_mm = pulse_us * 343 / 2000*/
    result->pulse_us = pulse_us;
    result->distance_mm = (pulse_us * 343) / 2000;
    result->valid = 1;
    return 0;
}

static int sr04_open(struct inode *inode, struct file *filp)
{
    struct miscdevice *miscdev;
    struct sr04_device *sr04;
    miscdev = filp->private_data;
    sr04 = container_of(miscdev,struct sr04_device,miscdev);
    filp->private_data = sr04;
    return 0;
}

static int sr04_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/*
 * ioctl 接口：
 *
 * 用户空间调用：
 *
 * struct sr04_measurement data;
 * ioctl(fd, SR04_IOC_GET_DISTANCE, &data);
 */
static long sr04_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
    struct sr04_device *sr04 = filp->private_data;
    struct sr04_measurement result;
    void __user *argp = (void __user *)arg;
    int ret;
    if (_IOC_TYPE(cmd) != SR04_IOC_MAGIC)
        return -ENOTTY;
    if (cmd != SR04_IOC_GET_DISTANCE)
        return -ENOTTY;
    ret = mutex_lock_interruptible(&sr04->measure_lock);
    if (ret)
        return ret;
    ret = sr04_do_measurement(sr04, &result);
    mutex_unlock(&sr04->measure_lock);
    if (ret)
        return ret;
    if (copy_to_user(argp, &result, sizeof(result)))
        return -EFAULT;

    return 0;
}

/*
 * read 接口：
 *
 * 每次打开后读取一次。
 *
 * 示例：
 *
 * cat /dev/sr04
 *
 * 输出：
 *
 * distance_mm=342 distance_cm=34.2 pulse_us=1994 valid=1
 */
static ssize_t sr04_read(struct file *filp,char __user *buf,size_t count,loff_t *ppos)
{
    struct sr04_device *sr04 = filp->private_data;
    struct sr04_measurement result;
    char output[128];
    int len;
    int ret;
    if (*ppos != 0)
        return 0;
    ret = mutex_lock_interruptible(&sr04->measure_lock);
    if (ret)
        return ret;
    ret = sr04_do_measurement(sr04, &result);
    mutex_unlock(&sr04->measure_lock);
    if (ret)
        return ret;
    len = scnprintf(output,
                    sizeof(output),
                    "distance_mm=%u distance_cm=%u.%u pulse_us=%u valid=%u\n",
                    result.distance_mm,
                    result.distance_mm / 10,
                    result.distance_mm % 10,
                    result.pulse_us,
                    result.valid);

    return simple_read_from_buffer(buf,count,ppos,output,len);
}

static const struct file_operations sr04_fops = {
    .owner = THIS_MODULE,
    .open = sr04_open,
    .release = sr04_release,
    .read = sr04_read,
    .unlocked_ioctl = sr04_ioctl,
    .llseek = no_llseek,
};

static int sr04_probe(struct platform_device *pdev)
{
    struct sr04_device *sr04;
    int ret;
    dev_info(&pdev->dev, "SR04 probe start\n");
    sr04 = devm_kzalloc(&pdev->dev,sizeof(*sr04),GFP_KERNEL);
    if (!sr04)
        return -ENOMEM;
    sr04->dev = &pdev->dev;
    mutex_init(&sr04->measure_lock);
    spin_lock_init(&sr04->state_lock);
    init_completion(&sr04->measurement_done);

    sr04->trig_gpio = devm_gpiod_get(&pdev->dev,"trig",GPIOD_OUT_LOW);
    if (IS_ERR(sr04->trig_gpio)) {
        ret = PTR_ERR(sr04->trig_gpio);
        dev_err(&pdev->dev,"failed to get trig GPIO: %d\n",ret);
        return ret;
    }

    sr04->echo_gpio = devm_gpiod_get(&pdev->dev,"echo",GPIOD_IN);
    if (IS_ERR(sr04->echo_gpio)) {
        ret = PTR_ERR(sr04->echo_gpio);
        dev_err(&pdev->dev,"failed to get echo GPIO: %d\n",ret);
        return ret;
    }

    sr04->irq = gpiod_to_irq(sr04->echo_gpio);
    if (sr04->irq < 0) {
        dev_err(&pdev->dev,"failed to convert echo GPIO to IRQ: %d\n",sr04->irq);
        return sr04->irq;
    }

    ret = devm_request_irq(&pdev->dev,sr04->irq,sr04_irq_handler,IRQF_TRIGGER_RISING |
                    IRQF_TRIGGER_FALLING,DRIVER_NAME,sr04);
    if (ret) {
        dev_err(&pdev->dev,"failed to request echo IRQ %d: %d\n",sr04->irq,ret);
        return ret;
    }

    sr04->miscdev.minor = MISC_DYNAMIC_MINOR;
    sr04->miscdev.name = DEVICE_NAME;
    sr04->miscdev.fops = &sr04_fops;
    sr04->miscdev.parent = &pdev->dev;

    ret = misc_register(&sr04->miscdev);
    if (ret) {
        dev_err(&pdev->dev,"failed to register misc device: %d\n",ret);
        return ret;
    }

    platform_set_drvdata(pdev, sr04);

    dev_info(&pdev->dev,"SR04 initialized successfully\n");
    dev_info(&pdev->dev,"device: /dev/%s, irq: %d\n",DEVICE_NAME,sr04->irq);
    return 0;
}

static int sr04_remove(struct platform_device *pdev)
{
    struct sr04_device *sr04;
    unsigned long flags;
    sr04 = platform_get_drvdata(pdev);
    misc_deregister(&sr04->miscdev);
    spin_lock_irqsave(&sr04->state_lock, flags);
    sr04->waiting_echo = false;
    sr04->rise_seen = false;
    spin_unlock_irqrestore(&sr04->state_lock, flags);
    synchronize_irq(sr04->irq);
    gpiod_set_value_cansleep(sr04->trig_gpio, 0);
    dev_info(&pdev->dev, "SR04 driver removed\n");
    return 0;
}

static const struct of_device_id sr04_of_match[] = {
    {
        .compatible = "smarthome,sr04",
    },
    {
    }
};

MODULE_DEVICE_TABLE(of, sr04_of_match);

static struct platform_driver sr04_driver = {
    .probe = sr04_probe,
    .remove = sr04_remove,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = sr04_of_match,
    },
};

module_platform_driver(sr04_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("JJL");
MODULE_DESCRIPTION("i.MX6ULL SR04 ultrasonic distance driver");