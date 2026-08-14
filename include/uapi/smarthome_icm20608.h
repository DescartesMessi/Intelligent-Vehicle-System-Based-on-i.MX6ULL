#ifndef SMARTHOME_ICM20608_H
#define SMARTHOME_ICM20608_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define ICM20608_DEVICE_NAME "icm20608"

#define ICM20608_IOC_MAGIC 'I'

struct icm20608_sample {
	__s16 accel_x;
	__s16 accel_y;
	__s16 accel_z;

	__s16 temperature;

	__s16 gyro_x;
	__s16 gyro_y;
	__s16 gyro_z;

	__u8 who_am_i;
	__u8 reserved[7];

	__u64 timestamp_ns;
};

#define ICM20608_IOC_GET_SAMPLE \
	_IOR(ICM20608_IOC_MAGIC, 0x01, struct icm20608_sample)

#define ICM20608_IOC_GET_WHO_AM_I \
	_IOR(ICM20608_IOC_MAGIC, 0x02, __u8)

#define ICM20608_IOC_RESET \
	_IO(ICM20608_IOC_MAGIC, 0x03)

#endif