#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0xc490865d, "struct_module" },
	{ 0xc9cb139f, "register_chrdev" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xfed004b2, "rt_fifo_user_read" },
	{ 0x287da078, "rt_fifo_user_write" },
	{ 0x1b7d4074, "printk" },
	{ 0x687c821e, "rt_fifo_user_ioctl" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=rtfifo";


MODULE_INFO(srcversion, "371FC576950EF49C122564C");
