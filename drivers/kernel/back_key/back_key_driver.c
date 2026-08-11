/*
 * back_key_driver.c
 * i.MX6ULL GPIO Back Key Driver
 * GPIO IRQ
 *     |
 * debounce timer
 *     |
 * workqueue
 *     |
 * Linux Input Subsystem
 *     |
 * /dev/input/eventX
 * 支持系统休眠唤醒
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/input.h>
#include <linux/pm_wakeup.h>
#include <linux/slab.h>

#define DRIVER_NAME         "smarthome-back-key"
#define DEFAULT_DEBOUNCE_MS 20

struct back_key_device {
    struct device *dev;
    struct gpio_desc *gpio;
    int irq;
    struct input_dev *input;
    unsigned int keycode;
    unsigned int debounce_ms;
    struct timer_list debounce_timer;
    struct work_struct report_work;
    bool last_state;
    bool wakeup_enabled;/* 是否允许该按键唤醒系统 */
    bool irq_wake_enabled;/* 当前 IRQ 是否已经设置为唤醒源 */
};

/* ============================================================
 * Workqueue
 * ============================================================
 */

static void back_key_report_work(struct work_struct *work)
{
    struct back_key_device *key;
    int value;
    bool pressed;
    key = container_of(work,struct back_key_device,report_work);
    value = gpiod_get_value_cansleep(key->gpio);
    if (value < 0) {
        dev_err(key->dev,
                "failed to read key gpio: %d\n",
                value);
        return;
    }

    /*
     * GPIO_ACTIVE_LOW 会由 GPIO descriptor 自动转换。
     *
     * pressed = true  表示按键按下
     * pressed = false 表示按键释放
     */
    pressed = !!value;
    if (pressed == key->last_state)
        return;
    key->last_state = pressed;
    input_report_key(key->input,key->keycode,pressed);
    input_sync(key->input);
    dev_dbg(key->dev,"BACK key %s\n",pressed ? "pressed" : "released");
}

/* ============================================================
 * Timer
 * ============================================================
 */

static void back_key_timer_function(unsigned long data)
{
    struct back_key_device *key;
    key = (struct back_key_device *)data;
    /*
     * timer 回调不能执行可能睡眠的 GPIO 操作，
     * 因此只负责调度 workqueue。
     */
    schedule_work(&key->report_work);
}

/* ============================================================
 * IRQ handler
 * ============================================================
 */

static irqreturn_t back_key_irq_handler(int irq,void *dev_id)
{
    struct back_key_device *key;
    key = dev_id;
    /*
     * 每次边沿到来后重新延迟 20 ms。
     * 如果机械抖动持续发生，定时器会不断被推迟。
     */
    mod_timer(&key->debounce_timer,jiffies +msecs_to_jiffies(key->debounce_ms));
    return IRQ_HANDLED;
}

/* ============================================================
 * Suspend / Resume
 * ============================================================
 */

static int back_key_suspend(struct device *dev)
{
    struct back_key_device *key;
    int ret;
    key = dev_get_drvdata(dev);
    if (!key)
        return 0;
    if (!device_may_wakeup(dev))
        return 0;
    if (key->irq_wake_enabled)
        return 0;
    ret = enable_irq_wake(key->irq);
    if (ret) {
        dev_err(dev,
                "enable_irq_wake failed: %d\n",
                ret);
        return ret;
    }
    key->irq_wake_enabled = true;
    dev_info(dev,"back key IRQ enabled as wakeup source\n");
    return 0;
}

static int back_key_resume(struct device *dev)
{
    struct back_key_device *key;
    int ret;
    key = dev_get_drvdata(dev);
    if (!key)
        return 0;
    if (!key->irq_wake_enabled)
        return 0;
    ret = disable_irq_wake(key->irq);
    if (ret) {
        dev_err(dev,"disable_irq_wake failed: %d\n",ret);
        return ret;
    }

    key->irq_wake_enabled = false;
    dev_info(dev,"back key IRQ wakeup disabled after resume\n");
    return 0;
}

static const struct dev_pm_ops back_key_pm_ops = {
    .suspend = back_key_suspend,
    .resume  = back_key_resume,
};

/* ============================================================
 * Probe
 * ============================================================
 */

static int back_key_probe(struct platform_device *pdev)
{
    struct back_key_device *key;
    struct device_node *np;
    u32 value;
    int ret;
    np = pdev->dev.of_node;
    dev_info(&pdev->dev,"back key probe start\n");
    key = devm_kzalloc(&pdev->dev,sizeof(*key),GFP_KERNEL);
    if (!key)
        return -ENOMEM;
    key->dev = &pdev->dev;
    platform_set_drvdata(pdev, key);
    /* 默认按键值为 KEY_BACK */
    key->keycode = KEY_BACK;
    /* 默认消抖时间 20 ms */
    key->debounce_ms = DEFAULT_DEBOUNCE_MS;
    /*
     * 从设备树读取 linux,code
     */
    ret = of_property_read_u32(np, "linux,code", &value);
    if (!ret)
        key->keycode = value;

    /*
     * 从设备树读取 debounce-interval
     */
    ret = of_property_read_u32(np,"debounce-interval",&value);
    if (!ret)
        key->debounce_ms = value;

    /*
     * 同时兼容旧的 gpio-key,wakeup
     * 和新的 wakeup-source 属性
     */
    key->wakeup_enabled =
        of_property_read_bool(np, "gpio-key,wakeup") ||
        of_property_read_bool(np, "wakeup-source");

    /*
     * 获取 key-gpios
     */
    key->gpio = devm_gpiod_get(&pdev->dev,"key",GPIOD_IN);
    if (IS_ERR(key->gpio)) {
        ret = PTR_ERR(key->gpio);
        dev_err(&pdev->dev,"failed to get key gpio: %d\n",ret);
        return ret;
    }

    /*
     * GPIO 转 IRQ
     */
    key->irq = gpiod_to_irq(key->gpio);
    if (key->irq < 0) {dev_err(&pdev->dev,"failed to get gpio irq: %d\n",key->irq);
        return key->irq;
    }

    /*
     * 分配 Input 设备
     */
    key->input = devm_input_allocate_device(&pdev->dev);
    if (!key->input) {
        dev_err(&pdev->dev,"failed to allocate input device\n");
        return -ENOMEM;
    }
    key->input->name = "smarthome-back-key";
    key->input->phys = "back-key/input0";
    key->input->id.bustype = BUS_HOST;
    input_set_capability(key->input,EV_KEY,key->keycode);
    /*
     * 初始化 workqueue
     */
    INIT_WORK(&key->report_work,back_key_report_work);
    /*
     * 初始化传统 timer
     */
    setup_timer(&key->debounce_timer,back_key_timer_function,(unsigned long)key);

    /*
     * 获取按键初始逻辑状态
     */
    ret = gpiod_get_value_cansleep(key->gpio);
    if (ret < 0) {
        dev_err(&pdev->dev,"failed to read initial GPIO: %d\n",ret);
        return ret;
    }

    key->last_state = !!ret;
    /*
     * 注册 Input 设备
     */
    ret = input_register_device(key->input);
    if (ret) {
        dev_err(&pdev->dev,"failed to register input device: %d\n",ret);
        return ret;
    }

    /*
     * 申请双边沿 IRQ
     */
    ret = devm_request_irq(&pdev->dev,key->irq,back_key_irq_handler,
        IRQF_TRIGGER_RISING |IRQF_TRIGGER_FALLING,DRIVER_NAME,key);
    if (ret) {
        dev_err(&pdev->dev,"failed to request irq %d: %d\n",key->irq,ret);
        return ret;
    }

    /*
     * 配置设备为可唤醒设备
     */
    device_init_wakeup(&pdev->dev,key->wakeup_enabled);
    dev_info(&pdev->dev,"back key initialized\n");
    dev_info(&pdev->dev,"GPIO IRQ = %d\n",key->irq);
    dev_info(&pdev->dev,"keycode = %u\n",key->keycode);
    dev_info(&pdev->dev,"debounce = %u ms\n",key->debounce_ms);
    dev_info(&pdev->dev,"wakeup = %s\n",
            key->wakeup_enabled ? "enabled" : "disabled");
    return 0;
}

/* ============================================================
 * Remove
 * ============================================================
 */

static int back_key_remove(struct platform_device *pdev)
{
    struct back_key_device *key;
    key = platform_get_drvdata(pdev);
    if (!key)
        return 0;
    if (key->irq_wake_enabled) {
        disable_irq_wake(key->irq);
        key->irq_wake_enabled = false;
    }
    device_init_wakeup(&pdev->dev, false);
    disable_irq(key->irq);
    del_timer_sync(&key->debounce_timer);
    cancel_work_sync(&key->report_work);
    dev_info(&pdev->dev,"back key driver removed\n");
    return 0;
}

/* ============================================================
 * Device Tree match
 * ============================================================
 */

static const struct of_device_id back_key_of_match[] = {
    {
        .compatible = "smarthome,back-key",
    },
    { }
};
MODULE_DEVICE_TABLE(of, back_key_of_match);
/* ============================================================
 * Platform Driver
 * ============================================================
 */
static struct platform_driver back_key_driver = {
    .probe  = back_key_probe,
    .remove = back_key_remove,
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = back_key_of_match,
        .pm = &back_key_pm_ops,
    },
};

module_platform_driver(back_key_driver);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("JJL");
MODULE_DESCRIPTION(
    "i.MX6ULL GPIO Back Key Input Driver with Wakeup Support"
);