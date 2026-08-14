ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-

BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

KERNEL_SOURCE ?= $(BSP_ROOT)/sources/linux-imx
KERNEL_BUILD ?= $(BSP_ROOT)/build/kernel
ROOTFS ?= $(BSP_ROOT)/deploy/nfs/rootfs

PROJECT_ROOT ?= $(abspath ../../..)
UAPI_DIR := $(PROJECT_ROOT)/include/uapi

MODULE_NAME := icm20608
MODULE_FILE := $(MODULE_NAME).ko

KERNEL_RELEASE := $(strip $(shell cat \
	$(KERNEL_BUILD)/include/config/kernel.release 2>/dev/null))

MODULE_INSTALL_DIR := \
	$(ROOTFS)/lib/modules/$(KERNEL_RELEASE)/extra

ccflags-y += -I$(UAPI_DIR)

obj-m += $(MODULE_NAME).o

.PHONY: all modules prepare install clean info

all: modules

modules:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules

prepare:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules_prepare

install: modules
	@test -n "$(KERNEL_RELEASE)" || \
		(echo "ERROR: kernel.release not found"; exit 1)

	@test -f "$(MODULE_FILE)" || \
		(echo "ERROR: $(MODULE_FILE) not found"; exit 1)

	mkdir -p "$(MODULE_INSTALL_DIR)"

	install -m 0644 \
		"$(MODULE_FILE)" \
		"$(MODULE_INSTALL_DIR)/$(MODULE_FILE)"

	@echo ""
	@echo "Installed:"
	@echo "$(MODULE_INSTALL_DIR)/$(MODULE_FILE)"

clean:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean

info:
	@echo "KERNEL_SOURCE  = $(KERNEL_SOURCE)"
	@echo "KERNEL_BUILD   = $(KERNEL_BUILD)"
	@echo "KERNEL_RELEASE = $(KERNEL_RELEASE)"
	@echo "ROOTFS         = $(ROOTFS)"
	@echo "PROJECT_ROOT   = $(PROJECT_ROOT)"
	@echo "UAPI_DIR       = $(UAPI_DIR)"
	@echo "MODULE_FILE    = $(MODULE_FILE)"
	@echo "INSTALL_DIR    = $(MODULE_INSTALL_DIR)"