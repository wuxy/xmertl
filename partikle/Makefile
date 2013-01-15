VERSION=0
PATCHLEVEL=2
SUBLEVEL=0
EXTRAVERSION=
KERNELRELEASE=$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

all: partikle

include config.mk

ifneq ($(findstring -j, $(MAKEFLAGS) ),-j)
MAKEFLAGS += -j3
endif

export KERNELRELEASE VERSION PATCHLEVEL SUBLEVEL EXTRAVERSION Q Q2 Q3 HOSTCC HOSTCFLAGS HOSTCFLAGS HOSTCXXFLAGS MAKEFLAGS
export PRTK USER_PATH ULIB_PATH ARCH CFLAGS CXXFLAGS ASFLAGS GCJFLAGS


# All the information about the host machine is gathered by the "ver_linux" script
SUBDIRS = core user scripts docs

scripts_basic: $(PRTK)/core/include/autoconf.h

config: rm_config
	+$(Q3)$(MAKE) -C scripts all
	@scripts/kconfig/conf Kconfig

menuconfig: rm_config
	@exec echo -n -e "\b \n>> Building the configuration utility:";
	+$(Q3)$(MAKE) -C scripts all
	@scripts/kconfig/mconf Kconfig

xconfig: rm_config
	@exec echo -e "\nNot implemented yet. Use make menuconfig instead."

oldconfig: rm_config
	+$(Q3)$(MAKE) -C scripts all Q=@
	+@scripts/kconfig/conf -o Kconfig

silentoldconfig: rm_config
	+$(Q3)$(MAKE) -C scripts all Q=@
	+$(Q3)scripts/kconfig/conf -s Kconfig

partikle: scripts_basic
	@exec echo ">> Detected PaRTiKle path: $(PRTK)"
	@exec echo -ne ">> Building PaRTiKle utils:"
	+$(Q3)$(MAKE) -C scripts all
	@exec echo -e  "\r>> Building PaRTiKle utils: done                 " # hide progress dots

	@exec echo -ne ">> Building PaRTiKle kernel [$(ARCH)]:"
	+$(Q3)$(MAKE) -C core all
	@exec echo -e  "\r>> Building PaRTiKle kernel [$(ARCH)]: done                                    "

	@exec echo -ne ">> Building PaRTiKle user libraries:"
	+$(Q3)$(MAKE) -C user all
	@exec echo -e  "\r>> Building PaRTiKle user libraries: done                  "

	@exec echo
	@exec echo ">> Include these in your profile environment:"
	@exec echo "   PRTK=$(PRTK) export PRTK"
	@exec echo '   PATH=$$PATH:$$PRTK/user/bin export PATH'


clean:
	@exec echo -ne ">> Cleaning up PaRTiKle:"
	$(Q)$(RM) -f *.o *~
	+@for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $(MAKECMDGOALS); \
	done;
	@exec echo -e  "\r>> Cleaning up PaRTiKle: done                           "

distclean: rm_config
	@exec echo -ne ">> Deep cleaning up PaRTiKle:"
	+@for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $(MAKECMDGOALS); \
	done;
# catch missing stale files
	$(Q)$(RM) $(shell find\
		-name "*.o" -o\
		-name ".*.cmd" -o\
		-name "*.a" -o\
		-name "*.jar" -o\
		-name "*.ali" -o\
		-name "*.prtk" -o\
		-name "*.bin" -o\
		-name dep.mk -o\
		-name "*" -type l)

	$(Q)$(RM) core/include/autoconf.h .config .config.old .tmpconfig.h .menuconfig.log;
	@exec echo -e "\r>> Deep cleaning up PaRTiKle: done                                       "

# in case the .config has been update clean up .config dependent parts
rm_config: #.config
	$(Q3)rm -f $(PRTK)/core/include/arch \
		$(PRTK)/user/ulibc/include/kernel_inc \
		$(PRTK)/user/ulibc/include/kernel_inc \
		$(PRTK)/user/ulibc/include/arch \
		$(PRTK)/user/bin/Config.scr \
		$(PRTK)/user/lang_support/include/arch



help:
	@echo -e "\nUsage: make [help|menuconfig|clean|distclean|env]\n"
	@echo -e "  help:       display this help and exit."
	@echo -e "  menuconfig: runs the configuration tool."
	@echo -e "  clean:      removes all the object files of partikle."
	@echo -e "  distclean:  removes any configuration and generated file from the source tree."
	@echo -e "  env:        display the values of the environment variables needed to build"
	@echo -e "              PaRTiKle applications.\n"

env:
	@exec echo "   PRTK=$(PRTK) export PRTK"
	@exec echo '   PATH=$$PATH:$$PRTK/user/bin export PATH'

.PHONY: help env
