BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp
CROSS_COMPILE ?= arm-linux-gnueabihf-

PROJECT_ROOT := $(abspath $(CURDIR)/../..)
UAPI_DIR := $(PROJECT_ROOT)/include/uapi

ROOTFS ?= $(BSP_ROOT)/deploy/nfs/rootfs
INSTALL_DIR := $(ROOTFS)/usr/local/bin

CC := $(CROSS_COMPILE)gcc
TARGET := test_sr501
SOURCE := test_sr501.c

CFLAGS := -O2 -Wall -Wextra -std=gnu99
CPPFLAGS := -I$(UAPI_DIR)

.PHONY: all install clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CPPFLAGS) $(CFLAGS) \
		$< \
		-o $@

install: $(TARGET)
	mkdir -p "$(INSTALL_DIR)"
	install -m 0755 \
		"$(TARGET)" \
		"$(INSTALL_DIR)/$(TARGET)"
	@echo "Installed: $(INSTALL_DIR)/$(TARGET)"

clean:
	rm -f "$(TARGET)"