deps_config := \
	core/Kconfig \
	core/kernel/xm_i386/Kconfig \
	core/kernel/linux/Kconfig \
	core/kernel/arm/Kconfig \
	core/kernel/i386/Kconfig \
	core/kernel/port/Kconfig \
	core/kernel/xm_i386/Kconfig.drv \
	core/kernel/linux/Kconfig.drv \
	core/kernel/arm/Kconfig.drv \
	core/kernel/i386/Kconfig.drv \
	core/kernel/port/Kconfig.drv \
	Kconfig

.config include/linux/autoconf.h: $(deps_config)

$(deps_config):
