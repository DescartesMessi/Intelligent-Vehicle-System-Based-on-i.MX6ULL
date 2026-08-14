// SPDX-License-Identifier: GPL-2.0
/*
 * Goodix GT9147 touchscreen driver
 *
 * Target platform:
 *   NXP i.MX6ULL
 *   Linux 4.x
 *
 * Interface:
 *   Linux input subsystem
 *   /dev/input/eventX
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>

#define GT9147_DRIVER_NAME          "gt9147"
#define GT9147_INPUT_NAME           "gt9147-touchscreen"

#define GT9147_REG_COMMAND          0x8040
#define GT9147_REG_CONFIG           0x8047
#define GT9147_REG_PRODUCT_ID       0x8140
#define GT9147_REG_STATUS           0x814e
#define GT9147_REG_POINTS           0x814f

#define GT9147_STATUS_READY         BIT(7)
#define GT9147_STATUS_TOUCH_MASK    0x0f

#define GT9147_POINT_SIZE           8
#define GT9147_MAX_TOUCHES          5

#define GT9147_DEFAULT_WIDTH        800
#define GT9147_DEFAULT_HEIGHT       480

struct gt9147_ts {
	struct i2c_client *client;
	struct input_dev *input;

	int reset_gpio;
	int irq_gpio;
	int irq;

	unsigned int size_x;
	unsigned int size_y;
	unsigned int max_touch_num;
	unsigned int irq_type;

	bool inverted_x;
	bool inverted_y;
	bool swapped_xy;
};

static const unsigned long gt9147_irq_flags[] = {
	IRQF_TRIGGER_RISING,
	IRQF_TRIGGER_FALLING,
	IRQF_TRIGGER_LOW,
	IRQF_TRIGGER_HIGH,
};

static int gt9147_i2c_read(struct gt9147_ts *ts, u16 reg,
			   u8 *buffer, unsigned int length)
{
	struct i2c_msg messages[2];
	u8 address[2];
	int ret;

	address[0] = reg >> 8;
	address[1] = reg & 0xff;

	messages[0].addr = ts->client->addr;
	messages[0].flags = 0;
	messages[0].len = sizeof(address);
	messages[0].buf = address;

	messages[1].addr = ts->client->addr;
	messages[1].flags = I2C_M_RD;
	messages[1].len = length;
	messages[1].buf = buffer;

	ret = i2c_transfer(ts->client->adapter, messages, 2);
	if (ret == 2)
		return 0;

	if (ret < 0)
		return ret;

	return -EIO;
}

static int gt9147_i2c_write(struct gt9147_ts *ts, u16 reg,
			    const u8 *buffer, unsigned int length)
{
	struct i2c_msg message;
	u8 *data;
	int ret;

	data = kmalloc(length + 2, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data[0] = reg >> 8;
	data[1] = reg & 0xff;

	if (length)
		memcpy(&data[2], buffer, length);

	message.addr = ts->client->addr;
	message.flags = 0;
	message.len = length + 2;
	message.buf = data;

	ret = i2c_transfer(ts->client->adapter, &message, 1);
	kfree(data);

	if (ret == 1)
		return 0;

	if (ret < 0)
		return ret;

	return -EIO;
}

static int gt9147_i2c_write_u8(struct gt9147_ts *ts, u16 reg, u8 value)
{
	return gt9147_i2c_write(ts, reg, &value, sizeof(value));
}

static int gt9147_reset(struct gt9147_ts *ts)
{
	int ret;

	/*
	 * Goodix address selection sequence:
	 *
	 * RESET low
	 * INT high selects I2C address 0x14
	 * INT low selects I2C address 0x5d
	 * RESET high
	 * INT low for synchronization
	 * INT becomes interrupt input
	 */

	ret = gpio_direction_output(ts->reset_gpio, 0);
	if (ret)
		return ret;

	ret = gpio_direction_output(ts->irq_gpio,
				    ts->client->addr == 0x14 ? 1 : 0);
	if (ret)
		return ret;

	msleep(20);

	gpio_set_value(ts->reset_gpio, 1);
	usleep_range(6000, 10000);

	gpio_set_value(ts->irq_gpio, 0);
	msleep(50);

	ret = gpio_direction_input(ts->irq_gpio);
	if (ret)
		return ret;

	msleep(10);

	return 0;
}

static int gt9147_read_device_info(struct gt9147_ts *ts)
{
	struct device_node *node = ts->client->dev.of_node;
	u8 product_data[6];
	u8 config_data[7];
	char product_id[5];
	u32 value;
	int ret;

	ret = gt9147_i2c_read(ts, GT9147_REG_PRODUCT_ID,
			      product_data, sizeof(product_data));
	if (ret) {
		dev_err(&ts->client->dev,
			"failed to read product ID: %d\n", ret);
		return ret;
	}

	memcpy(product_id, product_data, 4);
	product_id[4] = '\0';

	dev_info(&ts->client->dev,
		 "product ID: %s, firmware version: %02x%02x\n",
		 product_id, product_data[5], product_data[4]);

	ret = gt9147_i2c_read(ts, GT9147_REG_CONFIG,
			      config_data, sizeof(config_data));
	if (ret) {
		dev_err(&ts->client->dev,
			"failed to read configuration: %d\n", ret);
		return ret;
	}

	ts->size_x = config_data[1] | (config_data[2] << 8);
	ts->size_y = config_data[3] | (config_data[4] << 8);
	ts->max_touch_num = config_data[5] & 0x0f;
	ts->irq_type = config_data[6] & 0x03;

	if (!ts->size_x)
		ts->size_x = GT9147_DEFAULT_WIDTH;

	if (!ts->size_y)
		ts->size_y = GT9147_DEFAULT_HEIGHT;

	if (!ts->max_touch_num ||
	    ts->max_touch_num > GT9147_MAX_TOUCHES)
		ts->max_touch_num = GT9147_MAX_TOUCHES;

	/*
	 * Device-tree properties override the controller configuration.
	 * These values represent the final display coordinate dimensions.
	 */
	if (!of_property_read_u32(node, "touchscreen-size-x", &value) &&
	    value > 0)
		ts->size_x = value;

	if (!of_property_read_u32(node, "touchscreen-size-y", &value) &&
	    value > 0)
		ts->size_y = value;

	ts->inverted_x =
		of_property_read_bool(node, "touchscreen-inverted-x");
	ts->inverted_y =
		of_property_read_bool(node, "touchscreen-inverted-y");
	ts->swapped_xy =
		of_property_read_bool(node, "touchscreen-swapped-x-y");

	dev_info(&ts->client->dev,
		 "resolution: %ux%u, points: %u, irq type: %u\n",
		 ts->size_x, ts->size_y,
		 ts->max_touch_num, ts->irq_type);

	return 0;
}

static void gt9147_transform_position(struct gt9147_ts *ts,
				      unsigned int *x,
				      unsigned int *y)
{
	unsigned int temp;

	if (ts->swapped_xy) {
		temp = *x;
		*x = *y;
		*y = temp;
	}

	if (*x >= ts->size_x)
		*x = ts->size_x - 1;

	if (*y >= ts->size_y)
		*y = ts->size_y - 1;

	if (ts->inverted_x)
		*x = ts->size_x - 1 - *x;

	if (ts->inverted_y)
		*y = ts->size_y - 1 - *y;
}

static irqreturn_t gt9147_irq_thread(int irq, void *device_data)
{
	struct gt9147_ts *ts = device_data;
	unsigned long active_slots = 0;
	u8 points[GT9147_POINT_SIZE * GT9147_MAX_TOUCHES];
	u8 status;
	unsigned int touch_num;
	unsigned int index;
	unsigned int slot;
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int id;
	u8 *point;
	int ret;

	ret = gt9147_i2c_read(ts, GT9147_REG_STATUS,
			      &status, sizeof(status));
	if (ret) {
		dev_err_ratelimited(&ts->client->dev,
				    "failed to read status: %d\n", ret);
		return IRQ_HANDLED;
	}

	/*
	 * Bit 7 indicates that a complete coordinate report is ready.
	 * Spurious interrupts can occur, so do not process incomplete data.
	 */
	if (!(status & GT9147_STATUS_READY))
		return IRQ_HANDLED;

	touch_num = status & GT9147_STATUS_TOUCH_MASK;

	if (touch_num > ts->max_touch_num ||
	    touch_num > GT9147_MAX_TOUCHES) {
		dev_warn_ratelimited(&ts->client->dev,
				     "invalid touch count: %u\n",
				     touch_num);
		goto clear_status;
	}

	if (touch_num) {
		ret = gt9147_i2c_read(ts, GT9147_REG_POINTS,
				      points,
				      touch_num * GT9147_POINT_SIZE);
		if (ret) {
			dev_err_ratelimited(&ts->client->dev,
					    "failed to read points: %d\n",
					    ret);
			goto clear_status;
		}
	}

	for (index = 0; index < touch_num; index++) {
		point = &points[index * GT9147_POINT_SIZE];

		id = point[0] & 0x0f;
		if (id >= ts->max_touch_num)
			continue;

		x = point[1] | (point[2] << 8);
		y = point[3] | (point[4] << 8);
		width = point[5] | (point[6] << 8);

		gt9147_transform_position(ts, &x, &y);

		set_bit(id, &active_slots);

		input_mt_slot(ts->input, id);
		input_mt_report_slot_state(ts->input,
					   MT_TOOL_FINGER, true);
		input_report_abs(ts->input,
				 ABS_MT_POSITION_X, x);
		input_report_abs(ts->input,
				 ABS_MT_POSITION_Y, y);
		input_report_abs(ts->input,
				 ABS_MT_TOUCH_MAJOR, width);
		input_report_abs(ts->input,
				 ABS_MT_WIDTH_MAJOR, width);
	}

	/*
	 * GT9147 only reports currently active contacts. Release every slot
	 * which was not present in this frame.
	 */
	for (slot = 0; slot < ts->max_touch_num; slot++) {
		if (test_bit(slot, &active_slots))
			continue;

		input_mt_slot(ts->input, slot);
		input_mt_report_slot_state(ts->input,
					   MT_TOOL_FINGER, false);
	}

	/*
	 * Generate ABS_X, ABS_Y and BTN_TOUCH compatibility events as well.
	 * This allows both modern Qt evdevtouch and older applications to work.
	 */
	input_mt_report_pointer_emulation(ts->input, true);
	input_sync(ts->input);

clear_status:
	ret = gt9147_i2c_write_u8(ts, GT9147_REG_STATUS, 0);
	if (ret) {
		dev_err_ratelimited(&ts->client->dev,
				    "failed to clear status: %d\n", ret);
	}

	return IRQ_HANDLED;
}

static int gt9147_input_init(struct gt9147_ts *ts)
{
	struct input_dev *input;
	int ret;

	input = input_allocate_device();
	if (!input)
		return -ENOMEM;

	ts->input = input;

	input->name = GT9147_INPUT_NAME;
	input->phys = "input/gt9147";
	input->id.bustype = BUS_I2C;
	input->dev.parent = &ts->client->dev;

	__set_bit(EV_KEY, input->evbit);
	__set_bit(EV_ABS, input->evbit);
	__set_bit(BTN_TOUCH, input->keybit);
	__set_bit(INPUT_PROP_DIRECT, input->propbit);

	input_set_abs_params(input, ABS_X,
			     0, ts->size_x - 1, 0, 0);
	input_set_abs_params(input, ABS_Y,
			     0, ts->size_y - 1, 0, 0);

	input_set_abs_params(input, ABS_MT_POSITION_X,
			     0, ts->size_x - 1, 0, 0);
	input_set_abs_params(input, ABS_MT_POSITION_Y,
			     0, ts->size_y - 1, 0, 0);

	input_set_abs_params(input, ABS_MT_TOUCH_MAJOR,
			     0, 255, 0, 0);
	input_set_abs_params(input, ABS_MT_WIDTH_MAJOR,
			     0, 255, 0, 0);

	ret = input_mt_init_slots(input,
				  ts->max_touch_num,
				  INPUT_MT_DIRECT);
	if (ret) {
		dev_err(&ts->client->dev,
			"failed to initialize MT slots: %d\n", ret);
		goto free_input;
	}

	ret = input_register_device(input);
	if (ret) {
		dev_err(&ts->client->dev,
			"failed to register input device: %d\n", ret);
		goto free_input;
	}

	return 0;

free_input:
	input_free_device(input);
	ts->input = NULL;

	return ret;
}

static int gt9147_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct gt9147_ts *ts;
	struct device_node *node = client->dev.of_node;
	unsigned long irq_flags;
	int ret;

	if (!node)
		return -EINVAL;

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_I2C)) {
		dev_err(&client->dev,
			"I2C adapter does not support plain I2C\n");
		return -EIO;
	}

	ts = devm_kzalloc(&client->dev, sizeof(*ts), GFP_KERNEL);
	if (!ts)
		return -ENOMEM;

	ts->client = client;
	i2c_set_clientdata(client, ts);

	ts->reset_gpio =
		of_get_named_gpio(node, "reset-gpios", 0);
	if (ts->reset_gpio == -EPROBE_DEFER)
		return -EPROBE_DEFER;

	if (!gpio_is_valid(ts->reset_gpio)) {
		dev_err(&client->dev,
			"invalid reset-gpios property\n");
		return -EINVAL;
	}

	ts->irq_gpio =
		of_get_named_gpio(node, "interrupt-gpios", 0);
	if (ts->irq_gpio == -EPROBE_DEFER)
		return -EPROBE_DEFER;

	if (!gpio_is_valid(ts->irq_gpio)) {
		dev_err(&client->dev,
			"invalid interrupt-gpios property\n");
		return -EINVAL;
	}

	ret = devm_gpio_request_one(&client->dev,
				    ts->reset_gpio,
				    GPIOF_OUT_INIT_LOW,
				    "gt9147-reset");
	if (ret) {
		dev_err(&client->dev,
			"failed to request reset GPIO: %d\n", ret);
		return ret;
	}

	ret = devm_gpio_request_one(&client->dev,
				    ts->irq_gpio,
				    GPIOF_OUT_INIT_LOW,
				    "gt9147-interrupt");
	if (ret) {
		dev_err(&client->dev,
			"failed to request interrupt GPIO: %d\n", ret);
		return ret;
	}

	ret = gt9147_reset(ts);
	if (ret) {
		dev_err(&client->dev,
			"controller reset failed: %d\n", ret);
		return ret;
	}

	ret = gt9147_read_device_info(ts);
	if (ret)
		return ret;

	ret = gt9147_input_init(ts);
	if (ret)
		return ret;

	ts->irq = client->irq;
	if (ts->irq <= 0)
		ts->irq = gpio_to_irq(ts->irq_gpio);

	if (ts->irq < 0) {
		ret = ts->irq;
		dev_err(&client->dev,
			"failed to obtain IRQ: %d\n", ret);
		goto unregister_input;
	}

	irq_flags = gt9147_irq_flags[ts->irq_type] |
		    IRQF_ONESHOT;

	ret = devm_request_threaded_irq(&client->dev,
					ts->irq,
					NULL,
					gt9147_irq_thread,
					irq_flags,
					GT9147_DRIVER_NAME,
					ts);
	if (ret) {
		dev_err(&client->dev,
			"failed to request IRQ %d: %d\n",
			ts->irq, ret);
		goto unregister_input;
	}

	dev_info(&client->dev,
		 "GT9147 touchscreen registered on IRQ %d\n",
		 ts->irq);

	return 0;

unregister_input:
	input_unregister_device(ts->input);
	ts->input = NULL;

	return ret;
}

static int gt9147_remove(struct i2c_client *client)
{
	struct gt9147_ts *ts = i2c_get_clientdata(client);

	if (!ts)
		return 0;

	if (ts->irq > 0)
		devm_free_irq(&client->dev, ts->irq, ts);

	if (ts->input) {
		input_unregister_device(ts->input);
		ts->input = NULL;
	}

	return 0;
}

static const struct i2c_device_id gt9147_id_table[] = {
	{ "gt9147", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, gt9147_id_table);

static const struct of_device_id gt9147_of_match[] = {
	{ .compatible = "goodix,gt9147" },
	{ .compatible = "goodix,gt9xx" },
	{ }
};
MODULE_DEVICE_TABLE(of, gt9147_of_match);

static struct i2c_driver gt9147_driver = {
	.driver = {
		.name = GT9147_DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = gt9147_of_match,
	},
	.probe = gt9147_probe,
	.remove = gt9147_remove,
	.id_table = gt9147_id_table,
};

module_i2c_driver(gt9147_driver);

MODULE_AUTHOR("Pointer");
MODULE_DESCRIPTION("Goodix GT9147 touchscreen driver for i.MX6ULL");
MODULE_LICENSE("GPL");