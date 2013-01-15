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
	{ 0x4cf36189, "remap_pfn_range" },
	{ 0xd5e4f141, "mem_map" },
	{ 0xccde8e4a, "vmalloc_to_page" },
	{ 0xb6dcac40, "xm_shm_ioctl" },
	{ 0xfc9bd9a1, "xm_shm_open" },
	{ 0x5fc42947, "xm_shm_unlink" },
	{ 0x1cde884b, "cdev_add" },
	{ 0xf1143b2b, "cdev_init" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x1b7d4074, "printk" },
	{ 0x36b1a60c, "cdev_del" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=shm";


MODULE_INFO(srcversion, "4778EBFD015DA28B97AF155");
