#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xfa985410, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x51204841, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0xda113ac9, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0x67c2fa54, __VMLINUX_SYMBOL_STR(__copy_to_user) },
	{ 0x49fcab7e, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x1a9ebf5, __VMLINUX_SYMBOL_STR(mutex_lock_interruptible) },
	{ 0x88c18e3, __VMLINUX_SYMBOL_STR(gpiod_set_value_cansleep) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x45ef1bea, __VMLINUX_SYMBOL_STR(gpiod_direction_output) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x15711cf, __VMLINUX_SYMBOL_STR(gpiod_get_value) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0x59175f83, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0xb7cfa4a8, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x17d73699, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x3c8c7d13, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xfeb25d8b, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xb6828306, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xf3bb59b5, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x885b5400, __VMLINUX_SYMBOL_STR(__devm_gpiod_get) },
	{ 0x93e3940, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0xb91f4362, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xb37da9a1, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x495be299, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xe6e17706, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Csmarthome,dht11*");

MODULE_INFO(srcversion, "41C0C149FB40169D920E274");
