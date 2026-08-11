#ifndef _AP3216C_IOCTL_H_
#define _AP3216C_IOCTL_H_

#include <linux/types.h>
#include <linux/ioctl.h>

/*
 * 一次返回 AP3216C 的完整状态 (推荐 Qt 等上层应用优先使用)
 */
struct ap3216c_sample {
    __u16 ir;
    __u16 als;
    __u16 ps;

    /* 状态位：1 = 数据有效，0 = 红外过强导致数据可能无效 */
    __u8 ir_valid;
    __u8 ps_valid;

    /* 接近检测：1 = 检测到物体靠近，0 = 物体远离 */
    __u8 object_near;

    __u8 reserved;
};

#define AP3216C_IOC_MAGIC   'A'

#define AP3216C_GET_SAMPLE  _IOR(AP3216C_IOC_MAGIC, 0x01, struct ap3216c_sample)

/* 保留单独获取数据的接口 */
#define AP3216C_GET_PS      _IOR(AP3216C_IOC_MAGIC, 0x02, __u16)
#define AP3216C_GET_ALS     _IOR(AP3216C_IOC_MAGIC, 0x03, __u16)
#define AP3216C_GET_IR      _IOR(AP3216C_IOC_MAGIC, 0x04, __u16)

#endif /* _AP3216C_IOCTL_H_ */