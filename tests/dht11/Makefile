BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

PROJECT_ROOT := $(abspath $(CURDIR)/../..)
UAPI_DIR := $(PROJECT_ROOT)/include/uapi

ROOTFS ?= $(BSP_ROOT)/deploy/nfs/rootfs
INSTALL_DIR := $(ROOTFS)/usr/local/bin

TARGET := test_dht11
SOURCE := test_dht11.c

CFLAGS := -O2 -Wall -Wextra -std=gnu99
CPPFLAGS := -I$(UAPI_DIR)

.PHONY: all install clean

all: $(TARGET)

$(TARGET): $(SOURCE) \
	$(UAPI_DIR)/smarthome_dht11.h
	$(CC) $(CPPFLAGS) $(CFLAGS) \
		$(SOURCE) \
		-o $(TARGET)

install: $(TARGET)
	mkdir -p "$(INSTALL_DIR)"
	install -m 0755 \
		"$(TARGET)" \
		"$(INSTALL_DIR)/$(TARGET)"
	@echo "Installed: $(INSTALL_DIR)/$(TARGET)"

clean:
	rm -f "$(TARGET)"