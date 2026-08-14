BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp

CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

ROOTFS ?= $(BSP_ROOT)/deploy/nfs/rootfs
INSTALL_DIR := $(ROOTFS)/usr/local/bin

TARGET := test_back_key
SRC := test_back_key.c

CFLAGS := -O2 -Wall -Wextra -std=gnu99

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	install -m 0755 $(TARGET) $(INSTALL_DIR)/$(TARGET)

clean:
	rm -f $(TARGET)