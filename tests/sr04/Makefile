BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

ROOTFS ?= $(BSP_ROOT)/deploy/nfs/rootfs
INSTALL_DIR := $(ROOTFS)/usr/bin

TARGET := test_sr04
SRC := test_sr04.c

CFLAGS := -O2 -Wall -Wextra -std=gnu99
CPPFLAGS := -I../../include/uapi

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) \
		$< -o $@

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	install -m 0755 \
		$(TARGET) \
		$(INSTALL_DIR)/$(TARGET)

clean:
	rm -f $(TARGET)