#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#include <smarthome_dht11.h>

#define DHT11_DEVICE_NAME "dht11"
#define DHT11_CLASS_NAME "dht11"

#define DHT11_START_LOW_US 18000
#define DHT11_START_RELEASE_US 30
#define DHT11_LEVEL_TIMEOUT_US 100
#define DHT11_BIT_SAMPLE_US 40
#define DHT11_DATA_BITS 40
#define DHT11_DATA_BYTES 5

struct dht11_device {
    dev_t devt;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct gpio_desc *data_gpio;
    struct mutex lock;
};

static int dht11_wait_level(struct dht11_device *dev,
                            int expected,
                            unsigned int timeout_us)
{
    unsigned int elapsed;
    int value;

    for (elapsed = 0; elapsed < timeout_us; elapsed++) {
        value = gpiod_get_value(dev->data_gpio);

        if (value < 0)
            return value;

        if (value == expected)
            return 0;

        udelay(1);
    }

    return -ETIMEDOUT;
}

static int dht11_read_bit(struct dht11_device *dev, int *bit)
{
    int value;
    int ret;

    ret = dht11_wait_level(dev, 0, DHT11_LEVEL_TIMEOUT_US);
    if (ret)
        return ret;

    ret = dht11_wait_level(dev, 1, DHT11_LEVEL_TIMEOUT_US);
    if (ret)
        return ret;

    udelay(DHT11_BIT_SAMPLE_US);

    value = gpiod_get_value(dev->data_gpio);
    if (value < 0)
        return value;

    *bit = value ? 1 : 0;

    ret = dht11_wait_level(dev, 0, DHT11_LEVEL_TIMEOUT_US);
    if (ret)
        return ret;

    return 0;
}

static int dht11_read_frame(struct dht11_device *dev, u8 data[5])
{
    unsigned long flags;
    int ret;
    int bit;
    int index;
    int byte_index;
    int bit_index;

    memset(data, 0, DHT11_DATA_BYTES);

    ret = gpiod_direction_output(dev->data_gpio, 0);
    if (ret)
        return ret;

    msleep(18);

    gpiod_set_value_cansleep(dev->data_gpio, 1);


    udelay(DHT11_START_RELEASE_US);

    ret = gpiod_direction_input(dev->data_gpio);
    if (ret)
        return ret;

    /*
     * DHT11 的时序窗口很短。
     * 锁住当前进程，避免同一个设备被并发访问。
     * 不在这里关闭全局中断，避免影响系统其他设备。
     */
    local_irq_save(flags);

    ret = dht11_wait_level(dev, 0, DHT11_LEVEL_TIMEOUT_US);
    if (ret)
        goto out_irq;

    ret = dht11_wait_level(dev, 1, DHT11_LEVEL_TIMEOUT_US);
    if (ret)
        goto out_irq;

    ret = dht11_wait_level(dev, 0, DHT11_LEVEL_TIMEOUT_US);
    if (ret)
        goto out_irq;

    for (index = 0; index < DHT11_DATA_BITS; index++) {
        ret = dht11_read_bit(dev, &bit);
        if (ret)
            goto out_irq;

        byte_index = index / 8;
        bit_index = 7 - (index % 8);

        if (bit)
            data[byte_index] |= 1 << bit_index;
    }

out_irq:
    local_irq_restore(flags);

    gpiod_direction_input(dev->data_gpio);

    return ret;
}

static int dht11_measure(struct dht11_device *dev,
                         struct dht11_measurement *measurement)
{
    u8 data[DHT11_DATA_BYTES];
    u8 checksum;
    int ret;

    ret = dht11_read_frame(dev, data);
    if (ret)
        return ret;

    checksum = data[0] + data[1] + data[2] + data[3];

    if (checksum != data[4])
        return -EBADMSG;

    memset(measurement, 0, sizeof(*measurement));

    measurement->humidity_integer = data[0];
    measurement->humidity_decimal = data[1];
    measurement->temperature_integer = data[2];
    measurement->temperature_decimal = data[3];
    measurement->checksum = data[4];
    measurement->valid = 1;

    return 0;
}

static int dht11_open(struct inode *inode, struct file *filp)
{
    struct dht11_device *dev;

    dev = container_of(inode->i_cdev, struct dht11_device, cdev);
    filp->private_data = dev;

    return 0;
}

static ssize_t dht11_read(struct file *filp,
                          char __user *buffer,
                          size_t count,
                          loff_t *offset)
{
    struct dht11_device *dev;
    struct dht11_measurement measurement;
    int ret;

    (void)offset;

    if (count < sizeof(measurement))
        return -EINVAL;

    dev = filp->private_data;
    if (!dev)
        return -ENODEV;

    if (mutex_lock_interruptible(&dev->lock))
        return -ERESTARTSYS;

    ret = dht11_measure(dev, &measurement);

    mutex_unlock(&dev->lock);

    if (ret)
        return ret;

    if (copy_to_user(buffer, &measurement, sizeof(measurement)))
        return -EFAULT;

    return sizeof(measurement);
}

static long dht11_ioctl(struct file *filp,
                        unsigned int command,
                        unsigned long argument)
{
    struct dht11_device *dev;
    struct dht11_measurement measurement;
    int ret;

    dev = filp->private_data;
    if (!dev)
        return -ENODEV;

    if (_IOC_TYPE(command) != DHT11_IOC_MAGIC)
        return -ENOTTY;

    if (command != DHT11_IOC_GET_MEASUREMENT)
        return -ENOTTY;

    if (mutex_lock_interruptible(&dev->lock))
        return -ERESTARTSYS;

    ret = dht11_measure(dev, &measurement);

    mutex_unlock(&dev->lock);

    if (ret)
        return ret;

    if (copy_to_user((void __user *)argument,
                     &measurement,
                     sizeof(measurement)))
        return -EFAULT;

    return 0;
}

static int dht11_release(struct inode *inode, struct file *filp)
{
    (void)inode;
    (void)filp;

    return 0;
}

static const struct file_operations dht11_fops = {
    .owner = THIS_MODULE,
    .open = dht11_open,
    .read = dht11_read,
    .unlocked_ioctl = dht11_ioctl,
    .release = dht11_release,
};

static int dht11_probe(struct platform_device *pdev)
{
    struct dht11_device *dev;
    int ret;

    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    platform_set_drvdata(pdev, dev);

    dev->data_gpio = devm_gpiod_get(&pdev->dev,
                                    "dht11",
                                    GPIOD_IN);
    if (IS_ERR(dev->data_gpio)) {
        ret = PTR_ERR(dev->data_gpio);
        dev_err(&pdev->dev,
                "failed to get DHT11 data GPIO: %d\n",
                ret);
        return ret;
    }

    mutex_init(&dev->lock);

    ret = alloc_chrdev_region(&dev->devt,
                              0,
                              1,
                              DHT11_DEVICE_NAME);
    if (ret)
        return ret;

    cdev_init(&dev->cdev, &dht11_fops);
    dev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&dev->cdev, dev->devt, 1);
    if (ret)
        goto err_unregister_chrdev;

    dev->class = class_create(THIS_MODULE,
                              DHT11_CLASS_NAME);
    if (IS_ERR(dev->class)) {
        ret = PTR_ERR(dev->class);
        goto err_cdev_del;
    }

    dev->device = device_create(dev->class,
                                &pdev->dev,
                                dev->devt,
                                dev,
                                DHT11_DEVICE_NAME);
    if (IS_ERR(dev->device)) {
        ret = PTR_ERR(dev->device);
        goto err_class_destroy;
    }

    ret = gpiod_direction_input(dev->data_gpio);
    if (ret)
        goto err_device_destroy;

    dev_info(&pdev->dev,
             "DHT11 ready, device=/dev/%s\n",
             DHT11_DEVICE_NAME);

    return 0;

err_device_destroy:
    device_destroy(dev->class, dev->devt);

err_class_destroy:
    class_destroy(dev->class);

err_cdev_del:
    cdev_del(&dev->cdev);

err_unregister_chrdev:
    unregister_chrdev_region(dev->devt, 1);

    return ret;
}

static int dht11_remove(struct platform_device *pdev)
{
    struct dht11_device *dev;

    dev = platform_get_drvdata(pdev);

    device_destroy(dev->class, dev->devt);
    class_destroy(dev->class);
    cdev_del(&dev->cdev);
    unregister_chrdev_region(dev->devt, 1);

    dev_info(&pdev->dev, "DHT11 removed\n");

    return 0;
}

static const struct of_device_id dht11_of_match[] = {
    {
        .compatible = "smarthome,dht11",
    },
    {
        /* sentinel */
    },
};

MODULE_DEVICE_TABLE(of, dht11_of_match);

static struct platform_driver dht11_driver = {
    .probe = dht11_probe,
    .remove = dht11_remove,
    .driver = {
        .name = DHT11_DEVICE_NAME,
        .owner = THIS_MODULE,
        .of_match_table = dht11_of_match,
    },
};

module_platform_driver(dht11_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("smarthome");
MODULE_DESCRIPTION("DHT11 temperature and humidity driver");