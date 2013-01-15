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
	{ 0x1ea68eea, "rtl_do_trace" },
	{ 0xefea0087, "xmtrace_core_setup" },
	{ 0x1b7d4074, "printk" },
	{ 0xf1bc470b, "rtl_trace" },
	{ 0x50aa5e02, "rtl_event_descriptor" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=xm";


MODULE_INFO(srcversion, "8D70F48D0CB596C9B690C81");
