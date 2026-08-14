#ifndef SMARTHOME_SR501_H
#define SMARTHOME_SR501_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define SR501_IOC_MAGIC 'S'

struct sr501_status {
    __u8 detected;
    __u8 valid;
    __u16 reserved;
    __u32 sequence;
};

#define SR501_IOC_GET_STATUS \
    _IOR(SR501_IOC_MAGIC, 0x01, struct sr501_status)

#endif