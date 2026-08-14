#ifndef SMARTHOME_DHT11_H
#define SMARTHOME_DHT11_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define DHT11_IOC_MAGIC 'D'

struct dht11_measurement {
    __u8 humidity_integer;
    __u8 humidity_decimal;
    __u8 temperature_integer;
    __u8 temperature_decimal;
    __u8 checksum;
    __u8 valid;
    __u16 reserved;
};

#define DHT11_IOC_GET_MEASUREMENT \
    _IOR(DHT11_IOC_MAGIC, 0x01, struct dht11_measurement)

#endif