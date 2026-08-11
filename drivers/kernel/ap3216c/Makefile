BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

KERNEL_SOURCE ?= $(BSP_ROOT)/sources/linux-imx
KERNEL_BUILD  ?= $(BSP_ROOT)/build/kernel
ROOTFS        ?= $(BSP_ROOT)/deploy/nfs/rootfs

ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-

MODULE_NAME := liteon_ap3216c

# 用户空间和内核模块共享的 ioctl 头文件路径
ccflags-y += -I$(src)/home/pointer/imx6ull/projects/Vehicle-system/include/uapi

obj-m += $(MODULE_NAME).o

KERNEL_RELEASE := $(shell cat \
	$(KERNEL_BUILD)/include/config/kernel.release 2>/dev/null)

ifeq ($(strip $(KERNEL_RELEASE)),)
$(error Cannot find kernel release file in $(KERNEL_BUILD))
endif

.PHONY: all modules clean install

all: modules

modules:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean

install: modules
	mkdir -p \
		$(ROOTFS)/lib/modules/$(KERNEL_RELEASE)/extra

	install -m 0644 \
		$(MODULE_NAME).ko \
		$(ROOTFS)/lib/modules/$(KERNEL_RELEASE)/extra/$(MODULE_NAME).ko