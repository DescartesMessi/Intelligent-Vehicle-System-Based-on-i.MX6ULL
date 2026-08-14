BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

KERNEL_SOURCE ?= $(BSP_ROOT)/sources/linux-imx
KERNEL_BUILD ?= $(BSP_ROOT)/build/kernel
ROOTFS ?= $(BSP_ROOT)/deploy/nfs/rootfs

ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-

PROJECT_ROOT := $(abspath $(CURDIR)/../../..)
UAPI_DIR := $(PROJECT_ROOT)/include/uapi

MODULE_NAME := dht11
MODULE_FILE := $(MODULE_NAME).ko

KERNEL_RELEASE := $(strip $(shell cat \
	$(KERNEL_BUILD)/include/config/kernel.release 2>/dev/null))

ifeq ($(KERNEL_RELEASE),)
KERNEL_RELEASE := $(strip $(shell $(MAKE) -s \
	-C $(KERNEL_SOURCE) \
	O=$(KERNEL_BUILD) \
	ARCH=$(ARCH) \
	CROSS_COMPILE=$(CROSS_COMPILE) \
	kernelrelease 2>/dev/null))
endif

INSTALL_DIR := $(ROOTFS)/lib/modules/$(KERNEL_RELEASE)/extra

obj-m += $(MODULE_NAME).o

EXTRA_CFLAGS += -I$(UAPI_DIR)

.PHONY: all modules prepare clean install

all: modules

modules:
	@echo "UAPI_DIR=$(UAPI_DIR)"
	@test -f "$(UAPI_DIR)/smarthome_dht11.h" || \
		(echo "缺少 smarthome_dht11.h"; exit 1)
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		modules

prepare:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules_prepare

clean:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean

install: modules
	@test -n "$(KERNEL_RELEASE)" || \
		(echo "KERNEL_RELEASE 为空"; exit 1)
	@test -f "$(MODULE_FILE)" || \
		(echo "$(MODULE_FILE) 不存在"; exit 1)
	mkdir -p "$(INSTALL_DIR)"
	install -m 0644 \
		"$(MODULE_FILE)" \
		"$(INSTALL_DIR)/$(MODULE_FILE)"
	@echo "Installed: $(INSTALL_DIR)/$(MODULE_FILE)"