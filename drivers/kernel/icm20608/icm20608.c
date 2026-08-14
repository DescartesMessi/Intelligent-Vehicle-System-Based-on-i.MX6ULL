// SPDX-License-Identifier: GPL-2.0
/*
 * ICM20608 SPI driver for i.MX6ULL
 *
 * Device:
 *     /dev/icm20608
 *
 * SPI:mode 0 8 bits per word maximum 8 MHz
 */

#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/uaccess.h>

#include <../../../include/uapi/smarthome_icm20608.h>

#define ICM20608_REG_SMPLRT_DIV       0x19
#define ICM20608_REG_CONFIG           0x1a
#define ICM20608_REG_GYRO_CONFIG      0x1b
#define ICM20608_REG_ACCEL_CONFIG     0x1c
#define ICM20608_REG_ACCEL_CONFIG_2   0x1d

#define ICM20608_REG_ACCEL_XOUT_H     0x3b
#define ICM20608_REG_TEMP_OUT_H       0x41
#define ICM20608_REG_GYRO_XOUT_H      0x43

#define ICM20608_REG_USER_CTRL        0x6a
#define ICM20608_REG_PWR_MGMT_1       0x6b
#define ICM20608_REG_PWR_MGMT_2       0x6c
#define ICM20608_REG_WHO_AM_I         0x75

#define ICM20608_PWR_DEVICE_RESET     0x80
#define ICM20608_PWR_CLOCK_PLL        0x01
#define ICM20608_USER_I2C_IF_DIS      0x10

#define ICM20608_WHO_AM_I_VALUE       0xaf
#define ICM20608_WHO_AM_I_ALT_VALUE   0xae

#define ICM20608_SPI_MAX_HZ           8000000

struct icm20608_device {
	struct spi_device *spi;
	struct miscdevice miscdev;
	struct mutex lock;
	u8 who_am_i;
};

static int icm20608_read_reg(struct icm20608_device *dev,u8 reg, u8 *value)
{
	u8 command;
	int ret;
	command = reg | 0x80;
	ret = spi_write_then_read(dev->spi,&command,1,value,1);

	return ret;
}

static int icm20608_read_regs(struct icm20608_device *dev,u8 reg,u8 *buffer,
			            unsigned int length)
{
	u8 command;
	command = reg | 0x80;

	return spi_write_then_read(dev->spi,&command,1,buffer,length);
}

static int icm20608_write_reg(struct icm20608_device *dev,u8 reg,u8 value)
{
	u8 buffer[2];

	buffer[0] = reg & 0x7f;
	buffer[1] = value;

	return spi_write(dev->spi, buffer, sizeof(buffer));
}

static int icm20608_reset_and_configure(struct icm20608_device *dev)
{
	int ret;

	ret = icm20608_write_reg(dev,ICM20608_REG_PWR_MGMT_1,ICM20608_PWR_DEVICE_RESET);
	if (ret)
		return ret;

	msleep(100);

	ret = icm20608_write_reg(dev,ICM20608_REG_PWR_MGMT_1,ICM20608_PWR_CLOCK_PLL);
	if (ret)
		return ret;

	ret = icm20608_write_reg(dev,ICM20608_REG_PWR_MGMT_2,0x00);
	if (ret)
		return ret;

	/*
	 * Disable I2C interface and use SPI only.
	 */
	ret = icm20608_write_reg(dev,ICM20608_REG_USER_CTRL,ICM20608_USER_I2C_IF_DIS);
	if (ret)
		return ret;

	/*
	 * Gyroscope output rate:
	 *
	 * 1 kHz / (1 + SMPLRT_DIV)
	 * 0x09 gives approximately 100 Hz.
	 */
	ret = icm20608_write_reg(dev,ICM20608_REG_SMPLRT_DIV,0x09);
	if (ret)
		return ret;

	/*
	 * Gyroscope DLPF.
	 */
	ret = icm20608_write_reg(dev,ICM20608_REG_CONFIG,0x01);
	if (ret)
		return ret;

	/*
	 * Gyroscope full scale: +/-250 dps.
	 */
	ret = icm20608_write_reg(dev,ICM20608_REG_GYRO_CONFIG, 0x00);
	if (ret)
		return ret;

	/*
	 * Accelerometer full scale: +/-2 g.
	 */
	ret = icm20608_write_reg(dev,ICM20608_REG_ACCEL_CONFIG,0x00);
	if (ret)
		return ret;

	/*
	 * Accelerometer DLPF.
	 */
	ret = icm20608_write_reg(dev,ICM20608_REG_ACCEL_CONFIG_2,0x00);
	if (ret)
		return ret;

	msleep(20);

	return 0;
}

static s16 icm20608_be16_to_s16(const u8 *data)
{
	return (s16)(((u16)data[0] << 8) | data[1]);
}

static int icm20608_get_sample_locked(struct icm20608_device *dev,
				      struct icm20608_sample *sample)
{
	u8 data[14];
	int ret;

	memset(sample, 0, sizeof(*sample));

	ret = icm20608_read_regs(dev,
				 ICM20608_REG_ACCEL_XOUT_H,
				 data,
				 sizeof(data));
	if (ret)
		return ret;

	sample->accel_x = icm20608_be16_to_s16(&data[0]);
	sample->accel_y = icm20608_be16_to_s16(&data[2]);
	sample->accel_z = icm20608_be16_to_s16(&data[4]);

	sample->temperature = icm20608_be16_to_s16(&data[6]);

	sample->gyro_x = icm20608_be16_to_s16(&data[8]);
	sample->gyro_y = icm20608_be16_to_s16(&data[10]);
	sample->gyro_z = icm20608_be16_to_s16(&data[12]);

	sample->who_am_i = dev->who_am_i;
	sample->timestamp_ns = ktime_get_ns();

	return 0;
}

static int icm20608_open(struct inode *inode, struct file *file)
{
	struct miscdevice *miscdev;
	struct icm20608_device *dev;

	miscdev = file->private_data;
	dev = container_of(miscdev,
			   struct icm20608_device,
			   miscdev);

	file->private_data = dev;

	return 0;
}

static ssize_t icm20608_read(struct file *file,
			     char __user *buffer,
			     size_t count,
			     loff_t *position)
{
	struct icm20608_device *dev = file->private_data;
	struct icm20608_sample sample;
	int ret;

	if (count < sizeof(sample))
		return -EINVAL;

	mutex_lock(&dev->lock);

	ret = icm20608_get_sample_locked(dev, &sample);

	mutex_unlock(&dev->lock);

	if (ret)
		return ret;

	if (copy_to_user(buffer, &sample, sizeof(sample)))
		return -EFAULT;

	return sizeof(sample);
}

static long icm20608_ioctl(struct file *file,
			   unsigned int command,
			   unsigned long argument)
{
	struct icm20608_device *dev = file->private_data;
	struct icm20608_sample sample;
	u8 who_am_i;
	int ret;

	switch (command) {
	case ICM20608_IOC_GET_SAMPLE:
		mutex_lock(&dev->lock);

		ret = icm20608_get_sample_locked(dev, &sample);

		mutex_unlock(&dev->lock);

		if (ret)
			return ret;

		if (copy_to_user((void __user *)argument,
				 &sample,
				 sizeof(sample)))
			return -EFAULT;

		return 0;

	case ICM20608_IOC_GET_WHO_AM_I:
		who_am_i = dev->who_am_i;

		if (copy_to_user((void __user *)argument,
				 &who_am_i,
				 sizeof(who_am_i)))
			return -EFAULT;

		return 0;

	case ICM20608_IOC_RESET:
		mutex_lock(&dev->lock);

		ret = icm20608_reset_and_configure(dev);

		mutex_unlock(&dev->lock);

		return ret;

	default:
		return -ENOTTY;
	}
}

static const struct file_operations icm20608_fops = {
	.owner = THIS_MODULE,
	.open = icm20608_open,
	.read = icm20608_read,
	.unlocked_ioctl = icm20608_ioctl,
	.llseek = no_llseek,
};

static int icm20608_probe(struct spi_device *spi)
{
	struct icm20608_device *dev;
	u8 who_am_i;
	int ret;

	dev = devm_kzalloc(&spi->dev,
			   sizeof(*dev),
			   GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->spi = spi;
	mutex_init(&dev->lock);

	spi->mode = SPI_MODE_0;
	spi->bits_per_word = 8;

	if (!spi->max_speed_hz ||
	    spi->max_speed_hz > ICM20608_SPI_MAX_HZ)
		spi->max_speed_hz = ICM20608_SPI_MAX_HZ;

	ret = spi_setup(spi);
	if (ret) {
		dev_err(&spi->dev,
			"spi_setup failed: %d\n",
			ret);
		return ret;
	}

	spi_set_drvdata(spi, dev);

	msleep(100);

	ret = icm20608_read_reg(dev,
				ICM20608_REG_WHO_AM_I,
				&who_am_i);
	if (ret) {
		dev_err(&spi->dev,
			"failed to read WHO_AM_I: %d\n",
			ret);
		return ret;
	}

	if (who_am_i != ICM20608_WHO_AM_I_VALUE &&
	    who_am_i != ICM20608_WHO_AM_I_ALT_VALUE) {
		dev_err(&spi->dev,
			"invalid WHO_AM_I: 0x%02x\n",
			who_am_i);
		return -ENODEV;
	}

	dev->who_am_i = who_am_i;

	ret = icm20608_reset_and_configure(dev);
	if (ret) {
		dev_err(&spi->dev,
			"device configuration failed: %d\n",
			ret);
		return ret;
	}

	dev->miscdev.minor = MISC_DYNAMIC_MINOR;
	dev->miscdev.name = ICM20608_DEVICE_NAME;
	dev->miscdev.fops = &icm20608_fops;
	dev->miscdev.parent = &spi->dev;

	ret = misc_register(&dev->miscdev);
	if (ret) {
		dev_err(&spi->dev,
			"misc_register failed: %d\n",
			ret);
		return ret;
	}

	dev_info(&spi->dev,
		 "ICM20608 detected, WHO_AM_I=0x%02x, "
		 "SPI mode=%u, frequency=%u Hz\n",
		 dev->who_am_i,
		 spi->mode,
		 spi->max_speed_hz);

	return 0;
}

static int icm20608_remove(struct spi_device *spi)
{
	struct icm20608_device *dev;

	dev = spi_get_drvdata(spi);

	if (dev)
		misc_deregister(&dev->miscdev);

	return 0;
}

static const struct of_device_id icm20608_of_match[] = {
	{
		.compatible = "smarthome,icm20608",
	},
	{
		.compatible = "invensense,icm20608",
	},
	{ }
};
MODULE_DEVICE_TABLE(of, icm20608_of_match);

static const struct spi_device_id icm20608_id_table[] = {
	{
		.name = "icm20608",
	},
	{ }
};
MODULE_DEVICE_TABLE(spi, icm20608_id_table);

static struct spi_driver icm20608_driver = {
	.driver = {
		.name = "icm20608",
		.owner = THIS_MODULE,
		.of_match_table = icm20608_of_match,
	},
	.probe = icm20608_probe,
	.remove = icm20608_remove,
	.id_table = icm20608_id_table,
};

module_spi_driver(icm20608_driver);

MODULE_AUTHOR("Pointer");
MODULE_DESCRIPTION("ICM20608 SPI driver for i.MX6ULL");
MODULE_LICENSE("GPL");