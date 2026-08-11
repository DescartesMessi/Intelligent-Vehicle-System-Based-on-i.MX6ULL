BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

KERNEL_SOURCE ?= $(BSP_ROOT)/sources/linux-imx
KERNEL_BUILD  ?= $(BSP_ROOT)/build/kernel
ROOTFS        ?= $(BSP_ROOT)/deploy/nfs/rootfs

ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-

MODULE_NAME := back_key_driver

KERNEL_RELEASE := $(shell cat \
	$(KERNEL_BUILD)/include/config/kernel.release 2>/dev/null)

ifeq ($(strip $(KERNEL_RELEASE)),)
$(error Kernel release file not found)
endif

obj-m += $(MODULE_NAME).o

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