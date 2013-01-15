/*
 * Automatically generated C config: don't edit
 * PaRTiKle
 * Tue Nov 27 10:38:01 2012
 */
#define AUTOCONF_INCLUDED
#undef CONFIG_ARCH_X86
#undef CONFIG_ARCH_ARM
#undef CONFIG_ARCH_LINUX
#define CONFIG_ARCH_XTRATUM 1
#undef CONFIG_ARCH_LPC

/*
 * Devices
 */
#define CONFIG_PORT_DEVUART 1
#define CONFIG_XTRATUM_FIFO 1
#define CONFIG_XTRATUM_SHM 1

/*
 * Architecture specific
 */
#undef CONFIG_FP_SUPPORT

/*
 * Core Options
 */
#define CONFIG_KDYNAMIC_MEMORY 2097152
#define CONFIG_UDYNAMIC_MEMORY 1047552
#undef CONFIG_VERBOSE_COMPILE
#undef CONFIG_DEBUGGING

/*
 * Ulibc Options
 */
#define CONFIG_ULIBC_ERRSTR 1

/*
 * Language Support
 */
#undef CONFIG_ADA_SUPPORT
#define CONFIG_GNAT_PATH "/opt/gnat2005"
#undef CONFIG_CXX_SUPPORT
#undef CONFIG_JAVA_SUPPORT
