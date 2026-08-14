#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <smarthome_sr501.h>

#define SR501_DEVICE_NAME "sr501"
#define SR501_CLASS_NAME "sr501"
#define SR501_DEBOUNCE_MS 50

struct sr501_device {
    dev_t devt;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct gpio_desc *gpio;
    struct delayed_work work;
    struct fasync_struct *fasync;
    wait_queue_head_t waitq;
    spinlock_t lock;
    int irq;
    int detected;
    __u32 event_seq;
};

struct sr501_file_context {
    struct sr501_device *dev;
    __u32 seen_seq;
};

static int sr501_fasync(int fd, struct file *filp, int on);

static struct class *sr501_global_class;

static void sr501_report_state(struct sr501_device *dev)
{
    int value;
    unsigned long flags;
    int changed = 0;

    value = gpiod_get_value(dev->gpio);
    if (value < 0)
        return;

    value = value ? 1 : 0;

    spin_lock_irqsave(&dev->lock, flags);

    if (value != dev->detected) {
        dev->detected = value;
        dev->event_seq++;
        changed = 1;
    }

    spin_unlock_irqrestore(&dev->lock, flags);

    if (!changed)
        return;

    wake_up_interruptible(&dev->waitq);
    kill_fasync(&dev->fasync, SIGIO, POLL_IN);
}

static void sr501_work_handler(struct work_struct *work)
{
    struct delayed_work *delayed_work;
    struct sr501_device *dev;

    delayed_work = container_of(work, struct delayed_work, work);
    dev = container_of(delayed_work, struct sr501_device, work);

    sr501_report_state(dev);
}

static irqreturn_t sr501_irq_handler(int irq, void *data)
{
    struct sr501_device *dev = data;

    mod_delayed_work(system_wq,
                    &dev->work,
                    msecs_to_jiffies(SR501_DEBOUNCE_MS));

    return IRQ_HANDLED;
}

static int sr501_open(struct inode *inode, struct file *filp)
{
    struct sr501_device *dev;
    struct sr501_file_context *ctx;
    unsigned long flags;

    dev = container_of(inode->i_cdev, struct sr501_device, cdev);

    ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
    if (!ctx)
        return -ENOMEM;

    spin_lock_irqsave(&dev->lock, flags);
    ctx->seen_seq = dev->event_seq;
    spin_unlock_irqrestore(&dev->lock, flags);

    ctx->dev = dev;
    filp->private_data = ctx;

    return 0;
}

static int sr501_release(struct inode *inode, struct file *filp)
{
    struct sr501_file_context *ctx = filp->private_data;

    sr501_fasync(-1, filp, 0);
    kfree(ctx);

    return 0;
}

static ssize_t sr501_read(struct file *filp,
                          char __user *buffer,
                          size_t count,
                          loff_t *offset)
{
    struct sr501_file_context *ctx;
    struct sr501_device *dev;
    struct sr501_status status;
    unsigned long flags;
    int ret;

    if (count < sizeof(status))
        return -EINVAL;

    ctx = filp->private_data;
    if (!ctx || !ctx->dev)
        return -ENODEV;

    dev = ctx->dev;

    if (READ_ONCE(dev->event_seq) == ctx->seen_seq) {
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;

        ret = wait_event_interruptible(
            dev->waitq,
            READ_ONCE(dev->event_seq) != ctx->seen_seq);

        if (ret)
            return ret;
    }

    memset(&status, 0, sizeof(status));

    spin_lock_irqsave(&dev->lock, flags);
    status.detected = dev->detected ? 1 : 0;
    status.valid = 1;
    status.sequence = dev->event_seq;
    spin_unlock_irqrestore(&dev->lock, flags);

    if (copy_to_user(buffer, &status, sizeof(status)))
        return -EFAULT;

    ctx->seen_seq = status.sequence;

    return sizeof(status);
}

static long sr501_ioctl(struct file *filp,
                        unsigned int command,
                        unsigned long argument)
{
    struct sr501_file_context *ctx;
    struct sr501_device *dev;
    struct sr501_status status;
    unsigned long flags;

    ctx = filp->private_data;
    if (!ctx || !ctx->dev)
        return -ENODEV;

    dev = ctx->dev;

    if (_IOC_TYPE(command) != SR501_IOC_MAGIC)
        return -ENOTTY;

    switch (command) {
    case SR501_IOC_GET_STATUS:
        memset(&status, 0, sizeof(status));

        spin_lock_irqsave(&dev->lock, flags);
        status.detected = dev->detected ? 1 : 0;
        status.valid = 1;
        status.sequence = dev->event_seq;
        spin_unlock_irqrestore(&dev->lock, flags);

        if (copy_to_user((void __user *)argument,
                         &status,
                         sizeof(status)))
            return -EFAULT;

        return 0;

    default:
        return -ENOTTY;
    }
}

static unsigned int sr501_poll(struct file *filp, poll_table *wait)
{
    struct sr501_file_context *ctx;
    struct sr501_device *dev;
    unsigned int mask = 0;

    ctx = filp->private_data;
    if (!ctx || !ctx->dev)
        return POLLERR;

    dev = ctx->dev;

    poll_wait(filp, &dev->waitq, wait);

    if (READ_ONCE(dev->event_seq) != ctx->seen_seq)
        mask |= POLLIN | POLLRDNORM;

    return mask;
}

static int sr501_fasync(int fd, struct file *filp, int on)
{
    struct sr501_file_context *ctx;

    ctx = filp->private_data;
    if (!ctx || !ctx->dev)
        return -ENODEV;

    return fasync_helper(fd, filp, on, &ctx->dev->fasync);
}

static const struct file_operations sr501_fops = {
    .owner = THIS_MODULE,
    .open = sr501_open,
    .release = sr501_release,
    .read = sr501_read,
    .unlocked_ioctl = sr501_ioctl,
    .poll = sr501_poll,
    .fasync = sr501_fasync,
};

static int sr501_probe(struct platform_device *pdev)
{
    struct sr501_device *dev;
    int ret;

    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    platform_set_drvdata(pdev, dev);

    dev->gpio = devm_gpiod_get(&pdev->dev, "sr501", GPIOD_IN);
    if (IS_ERR(dev->gpio)) {
        dev_err(&pdev->dev, "failed to get sr501 GPIO\n");
        return PTR_ERR(dev->gpio);
    }

    dev->irq = gpiod_to_irq(dev->gpio);
    if (dev->irq < 0) {
        dev_err(&pdev->dev, "failed to convert GPIO to IRQ\n");
        return dev->irq;
    }

    spin_lock_init(&dev->lock);
    init_waitqueue_head(&dev->waitq);
    INIT_DELAYED_WORK(&dev->work, sr501_work_handler);

    dev->detected = gpiod_get_value(dev->gpio);
    if (dev->detected < 0)
        dev->detected = 0;

    ret = alloc_chrdev_region(&dev->devt,
                              0,
                              1,
                              SR501_DEVICE_NAME);
    if (ret)
        return ret;

    cdev_init(&dev->cdev, &sr501_fops);
    dev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&dev->cdev, dev->devt, 1);
    if (ret)
        goto err_unregister_chrdev;

    if (!sr501_global_class) {
        sr501_global_class = class_create(THIS_MODULE,
                                           SR501_CLASS_NAME);
        if (IS_ERR(sr501_global_class)) {
            ret = PTR_ERR(sr501_global_class);
            sr501_global_class = NULL;
            goto err_cdev_del;
        }
    }

    dev->class = sr501_global_class;

    dev->device = device_create(dev->class,
                                &pdev->dev,
                                dev->devt,
                                dev,
                                SR501_DEVICE_NAME);
    if (IS_ERR(dev->device)) {
        ret = PTR_ERR(dev->device);
        dev->device = NULL;
        goto err_class_destroy;
    }

    ret = devm_request_irq(&pdev->dev,
                           dev->irq,
                           sr501_irq_handler,
                           IRQF_TRIGGER_RISING |
                           IRQF_TRIGGER_FALLING,
                           SR501_DEVICE_NAME,
                           dev);
    if (ret) {
        dev_err(&pdev->dev,
                "failed to request IRQ %d: %d\n",
                dev->irq,
                ret);
        goto err_device_destroy;
    }

    dev_info(&pdev->dev,
             "SR501 ready, irq=%d, initial=%d\n",
             dev->irq,
             dev->detected);

    return 0;

err_device_destroy:
    device_destroy(dev->class, dev->devt);

err_class_destroy:
    if (sr501_global_class) {
        class_destroy(sr501_global_class);
        sr501_global_class = NULL;
    }

err_cdev_del:
    cdev_del(&dev->cdev);

err_unregister_chrdev:
    unregister_chrdev_region(dev->devt, 1);

    return ret;
}

static int sr501_remove(struct platform_device *pdev)
{
    struct sr501_device *dev;

    dev = platform_get_drvdata(pdev);

    cancel_delayed_work_sync(&dev->work);
    kill_fasync(&dev->fasync, SIGIO, POLL_HUP);

    device_destroy(dev->class, dev->devt);

    cdev_del(&dev->cdev);
    unregister_chrdev_region(dev->devt, 1);

    if (sr501_global_class) {
        class_destroy(sr501_global_class);
        sr501_global_class = NULL;
    }

    dev_info(&pdev->dev, "SR501 removed\n");

    return 0;
}

static const struct of_device_id sr501_of_match[] = {
    {
        .compatible = "smarthome,sr501",
    },
    {
        .compatible = "smarthome_sr501",
    },
    {
        /* sentinel */
    },
};

MODULE_DEVICE_TABLE(of, sr501_of_match);

static struct platform_driver sr501_driver = {
    .probe = sr501_probe,
    .remove = sr501_remove,
    .driver = {
        .name = SR501_DEVICE_NAME,
        .owner = THIS_MODULE,
        .of_match_table = sr501_of_match,
    },
};

module_platform_driver(sr501_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("smarthome");
MODULE_DESCRIPTION("SR501 PIR motion sensor driver");