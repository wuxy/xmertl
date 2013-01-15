.DEFAULT: all

#all:
#	echo "Miguel: Utiliza nombres de configuracion que empiecen por CONFIG_"
#	echo "Para que luego se puede pasar al sistema de config de menuconfig"

all: Rules.mk Config.scr xtratum

ifeq (./Config.mk,$(wildcard ./Config.mk))
include Config.mk
else
XM_ARCH=i386
LINUX_PATH=$(shell pwd)/linux
endif

ifndef XM_PATH
XM_PATH:=$(shell pwd)
endif

ifndef XMDEV_PATH
XMDEV_PATH:=$(shell pwd)/devs
endif

ifdef CROSS_COMPILE

CC=${CROSS_COMPILE}gcc
LD=${CROSS_COMPILE}ld
OBJCOPY=${CROSS_COMPILE}objcopy
else
CC=/usr/bin/gcc
LD=/usr/bin/ld
OBJCOPY=/usr/bin/objcopy
endif

EXTRA_CFLAGS = -Wall -O2 -D$(XM_ARCH) -I$(XM_PATH)/include -I$(LINUX_PATH)/include/asm/mach-default

LIBGCC = `${CC} -print-libgcc-file-name`

.PHONY: clean cleanall

links:
	@if [ ! -d $(XM_PATH)/include/arch ] ; then \
		ln -sf $(XM_PATH)/include/$(XM_ARCH) $(XM_PATH)/include/arch; \
	fi

obj-m:=xm.o

xm-objs:=

include $(XM_PATH)/arch/$(XM_ARCH)/kernel/files.mk
include $(XM_PATH)/kernel/files.mk

obj-m+=xmtrace/xmtrace_shm.o

export XM_PATH XM_ARCH XMDEV_PATH LINUX_PATH

xtratum: links
	@echo -e "\nDon't worry about the following \"__udivdi3\" and \"__umoddi3\" warnings.\n"
	@make -C $(LINUX_PATH) M=$(XM_PATH) modules
	@make -C $(XM_PATH)/user_tools all
	@$(LD) -r -o xtratum.ko xm.ko --start-group $(LIBGCC) --end-group
	@$(RM) xm.ko
	@mv xtratum.ko xm.ko
	@make -C $(LINUX_PATH) M=$(XMDEV_PATH) modules

Rules.mk: links
	@exec echo "# This file has been created automatically" > user_tools/Rules.mk
	@exec echo -e "# Please, don't change anything\n" >> user_tools/Rules.mk
	@exec echo "XM_PATH=$(XM_PATH)" >> user_tools/Rules.mk;
	@exec echo "USER_TOOLS_PATH=$(XM_PATH)/user_tools" >> user_tools/Rules.mk;
	@exec echo "XM_ARCH=$(XM_ARCH)" >> user_tools/Rules.mk
	@exec echo "LIBGCC=\`$(CC) -print-libgcc-file-name\`" >> user_tools/Rules.mk
	@exec echo -e 'CFLAGS=-fomit-frame-pointer -O2 -Wall -I$$(USER_TOOLS_PATH)/libxm/include -DUXMLIB' >> user_tools/Rules.mk
	@exec echo "CC=$(CC)" >> user_tools/Rules.mk
	@exec echo "LD=$(LD)" >> user_tools/Rules.mk
	@exec echo -e 'DEPEND=$$(CC) $$(CFLAGS) -M\n' >> user_tools/Rules.mk
	@exec echo ".PHONY: depend clean" >> user_tools/Rules.mk
	@exec echo 'dep.mk: $$(SRCS)' >> user_tools/Rules.mk
	@exec echo -e '\t@for file in $$(SRCS) ; do \\' >> user_tools/Rules.mk
	@exec echo -e '\t\t$$(DEPEND) $$$$file ; \\' >> user_tools/Rules.mk
	@exec echo -e '\tdone > dep.mk\n' >> user_tools/Rules.mk

SCRIPTS_PATH=$(XM_PATH)/user_tools/scripts
SCRIPTS_CONFIG=$(SCRIPTS_PATH)/Config.scr

Config.scr:
	@exec echo "# This file has been created automatically" > $(SCRIPTS_CONFIG)
	@exec echo -e "# Please, don't change anything\n" >> $(SCRIPTS_CONFIG)
	@exec echo "XM_PATH=$(XM_PATH)" >> $(SCRIPTS_CONFIG)
	@exec echo "USER_TOOLS_PATH=$(XM_PATH)/user_tools" >> $(SCRIPTS_CONFIG)
	@exec echo "XM_ARCH=$(XM_ARCH)" >> $(SCRIPTS_CONFIG)
	@exec echo "LIBGCC=\`gcc -print-libgcc-file-name\`" >> $(SCRIPTS_CONFIG)
	@exec echo "CC=$(CC)" >> $(SCRIPTS_CONFIG)
	@exec echo "LD=$(LD)" >> $(SCRIPTS_CONFIG)
	@exec echo "OBJCOPY=$(OBJCOPY)" >> $(SCRIPTS_CONFIG)
	@chmod +x $(SCRIPTS_PATH)/mkdomain

clean:
	-@make -C $(LINUX_PATH) M=$(XM_PATH) clean
	@make -C $(XM_PATH)/user_tools clean
	@make -C $(XMDEV_PATH) clean
	@$(RM) -f *.mod.c *.o *~ *.ko Modules.symvers include/arch $(USER_TOOLS_PATH)/Rules.mk \
		$(SCRIPTS_CONFIG)
	@find -name "*.o" -exec rm '{}' \;
	@find -name "*~" -exec rm '{}' \;
	@find -name "loader.xm" -exec rm '{}' \;
	@find -name "unloader.xm" -exec rm '{}' \;

distclean: clean
