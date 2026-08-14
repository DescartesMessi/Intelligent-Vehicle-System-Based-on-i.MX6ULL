# 把 smarthome_beep.c 编译成适配当前 Linux 4.1.15 内核的 ARM 内核模块，并安装到 rootfs 中。
#定义 BSP 工程根目录。?= 表示：如果外部没有传入该变量，就使用默认值。
BSP_ROOT ?= /home/pointer/imx6ull/projects/imx6ull-nxp-bsp
#定义 Linux 内核路径,KERNEL_SOURCE -> Linux 内核源码目录,KERNEL_BUILD  -> Linux 内核独立编译输出目录
KERNEL_SOURCE ?= $(BSP_ROOT)/sources/linux-imx
KERNEL_BUILD  ?= $(BSP_ROOT)/build/kernel
ROOTFS        ?= $(BSP_ROOT)/deploy/nfs/rootfs
# 目标架构：ARM ;交叉编译器前缀：arm-linux-gnueabihf-
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-

MODULE_NAME := smarthome_beep
KERNEL_RELEASE := $(shell cat \
	$(KERNEL_BUILD)/include/config/kernel.release 2>/dev/null)

obj-m += $(MODULE_NAME).o
#表示这些名称不是实际文件，而是 Makefile 的操作目标。可以执行：make\make modules\make clean\make install
.PHONY: all modules clean install

all: modules
#编译内核模块:进入 Linux 内核源码目录,使用 build/kernel 中的内核配置和编译结果,把当前目录作为外部模块目录
#使用 ARM 交叉编译器,编译 smarthome_beep.ko
modules:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		modules
#清理模块;只清理当前蜂鸣器模块的中间文件，例如：
clean:
	$(MAKE) -C $(KERNEL_SOURCE) \
		O=$(KERNEL_BUILD) \
		M=$(CURDIR) \
		ARCH=$(ARCH) \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		clean
#安装模块到 rootfs 中;创建目录 $(ROOTFS)/lib/modules/$(KERNEL_RELEASE)/extra,并把 smarthome_beep.ko 安装到该目录下
install: modules
	mkdir -p $(ROOTFS)/lib/modules/$(KERNEL_RELEASE)/extra
	install -m 0644 \
		$(MODULE_NAME).ko \
		$(ROOTFS)/lib/modules/$(KERNEL_RELEASE)/extra/$(MODULE_NAME).ko