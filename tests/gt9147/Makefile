CROSS_COMPILE ?= arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc

PROJECT_ROOT := $(abspath ../..)
ROOTFS ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp/deploy/nfs/rootfs

TARGET := gt9147_test
SOURCE := gt9147_test.c

CPPFLAGS += -I$(PROJECT_ROOT)/include/uapi
CFLAGS += -O2 -Wall -Wextra

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

install: $(TARGET)
	mkdir -p $(ROOTFS)/usr/local/bin
	install -m 0755 $(TARGET) $(ROOTFS)/usr/local/bin/$(TARGET)

clean:
	rm -f $(TARGET)