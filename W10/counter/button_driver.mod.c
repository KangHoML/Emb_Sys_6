#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd4b13e21, "module_layout" },
	{ 0xc793df81, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x551ab123, "class_destroy" },
	{ 0xc6bfeb6c, "device_destroy" },
	{ 0xfe990052, "gpio_free" },
	{ 0xf28855b, "gpiod_direction_input" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xf355108, "cdev_add" },
	{ 0x4392e2ef, "cdev_init" },
	{ 0x12284cf8, "device_create" },
	{ 0x42824447, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x6f414e08, "gpiod_get_raw_value" },
	{ 0x3a109e75, "gpio_to_desc" },
	{ 0x92997ed8, "_printk" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "82732C6EB5C454B41816005");
