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
	{ 0xab5c8cb5, __VMLINUX_SYMBOL_STR(i2c_del_driver) },
	{ 0x19bdcb34, __VMLINUX_SYMBOL_STR(i2c_register_driver) },
	{ 0x676bbc0f, __VMLINUX_SYMBOL_STR(_set_bit) },
	{ 0x226412e2, __VMLINUX_SYMBOL_STR(dev_warn) },
	{ 0x1005a311, __VMLINUX_SYMBOL_STR(input_mt_report_pointer_emulation) },
	{ 0xddaa6aba, __VMLINUX_SYMBOL_STR(input_mt_report_slot_state) },
	{ 0x8e117d54, __VMLINUX_SYMBOL_STR(input_event) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xcca6874c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0x208614a6, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x72350130, __VMLINUX_SYMBOL_STR(___ratelimit) },
	{ 0x94616f8b, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0x2770f6b8, __VMLINUX_SYMBOL_STR(devm_request_threaded_irq) },
	{ 0xe3c07ed7, __VMLINUX_SYMBOL_STR(input_register_device) },
	{ 0x1f592b1a, __VMLINUX_SYMBOL_STR(input_free_device) },
	{ 0xbedc9d6d, __VMLINUX_SYMBOL_STR(input_mt_init_slots) },
	{ 0x1e8b2bb, __VMLINUX_SYMBOL_STR(input_set_abs_params) },
	{ 0x243100ec, __VMLINUX_SYMBOL_STR(input_allocate_device) },
	{ 0x18e87da3, __VMLINUX_SYMBOL_STR(of_find_property) },
	{ 0xae372752, __VMLINUX_SYMBOL_STR(of_property_read_u32_array) },
	{ 0xb91f4362, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0xb7cfa4a8, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0x12a38747, __VMLINUX_SYMBOL_STR(usleep_range) },
	{ 0x389329b3, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x59175f83, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0xc61ff1f2, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x3089e8ae, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x1352f55c, __VMLINUX_SYMBOL_STR(devm_gpio_request_one) },
	{ 0xde6fb46c, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x93e3940, __VMLINUX_SYMBOL_STR(devm_kmalloc) },
	{ 0x243f207d, __VMLINUX_SYMBOL_STR(i2c_transfer) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
	{ 0xf8a905d3, __VMLINUX_SYMBOL_STR(input_unregister_device) },
	{ 0x5440f526, __VMLINUX_SYMBOL_STR(devm_free_irq) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cgoodix,gt9147*");
MODULE_ALIAS("of:N*T*Cgoodix,gt9xx*");
MODULE_ALIAS("i2c:gt9147");

MODULE_INFO(srcversion, "458D3920E6F793082EDFCED");
