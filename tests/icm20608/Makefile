BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

PROJECT_ROOT ?= $(abspath ../..)

CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

ROOTFS ?= $(BSP_ROOT)/deploy/nfs/rootfs
INSTALL_DIR := $(ROOTFS)/usr/local/bin

TARGET := test_icm20608
SOURCE := test_icm20608.c

UAPI_DIR := $(PROJECT_ROOT)/include/uapi

CPPFLAGS := -I$(UAPI_DIR)
CFLAGS := -O2 -Wall -Wextra -std=gnu99

.PHONY: all install clean info

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) \
		$(CPPFLAGS) \
		$(CFLAGS) \
		$< \
		-o $@

install: $(TARGET)
	mkdir -p "$(INSTALL_DIR)"

	install -m 0755 \
		"$(TARGET)" \
		"$(INSTALL_DIR)/$(TARGET)"

	@echo ""
	@echo "Installed:"
	@echo "$(INSTALL_DIR)/$(TARGET)"

clean:
	rm -f "$(TARGET)"

info:
	@echo "PROJECT_ROOT = $(PROJECT_ROOT)"
	@echo "UAPI_DIR     = $(UAPI_DIR)"
	@echo "ROOTFS       = $(ROOTFS)"
	@echo "INSTALL_DIR  = $(INSTALL_DIR)"
	@echo "TARGET       = $(TARGET)"