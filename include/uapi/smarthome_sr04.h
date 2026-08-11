#ifndef SMARTHOME_SR04_H
#define SMARTHOME_SR04_H

#include <linux/types.h>
#include <linux/ioctl.h>

// distance_mm：毫米;pulse_us：Echo 高电平持续时间;valid：1 表示测量有效，0 表示无效;
struct sr04_measurement {
    __u32 distance_mm;
    __u32 pulse_us;
    __u8  valid;
    __u8  reserved[3];
};

#define SR04_IOC_MAGIC       'S'

#define SR04_IOC_GET_DISTANCE \
    _IOR(SR04_IOC_MAGIC, 0x01, struct sr04_measurement)

#endif