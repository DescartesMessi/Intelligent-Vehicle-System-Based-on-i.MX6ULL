VEHICLE_ROOT := /home/pointer/imx6ull/projects/Vehicle-system
BSP_ROOT     := /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

SYSROOT      := $(BSP_ROOT)/build/sysroot
ROOTFS       := $(BSP_ROOT)/deploy/nfs/rootfs

CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

TARGET := test_liteon_ap3216c
SRC := test_liteon_ap3216c.c

CFLAGS := \
	-O2 \
	-Wall \
	-Wextra \
	--sysroot=$(SYSROOT) \
	-I$(VEHICLE_ROOT)/include/uapi

LDFLAGS := \
	--sysroot=$(SYSROOT)

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SRC) \
	$(VEHICLE_ROOT)/include/uapi/liteon_ap3216c.h
	$(CC) $(CFLAGS) $(SRC) \
	-o $(TARGET) \
	$(LDFLAGS)

install: $(TARGET)
	install -D -m 0755 \
	$(TARGET) \
	$(ROOTFS)/usr/bin/$(TARGET)

clean:
	rm -f $(TARGET)
