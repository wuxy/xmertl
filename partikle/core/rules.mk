# usage: include rules.mk, from Makefiles
# this file contains all common definitions related to core

# Extract core relevant config options from ".config" file
GNAT_PATH	= $(CONFIG_GNAT_PATH)
GNATVERSION	= GAP2005

CFLAGS= -Wall -O2 -D_KERNEL_ -fno-builtin -nostdinc -D$(ARCH) -I$(PRTK)/core/include -fno-strict-aliasing # -nostdlib
ASFLAGS= -Wall -O2 -D__ASSEMBLY__ -D_KERNEL_ -fno-builtin -D$(ARCH) -I$(PRTK)/core/include -nostdinc # -nostdlib

# set up compiler toolchain
ifdef ARCH
include $(PRTK)/mkfiles/mkfile-$(ARCH)
endif

CFLAGS += $(call check_gcc, -Wno-pointer-sign,)
CFLAGS += $(call check_gcc, -fno-stack-protector,)

ifdef CONFIG_DEBUGGING
CFLAGS+=-g
else
CFLAGS+=-fomit-frame-pointer
endif

.PHONY: depend clean
dep.mk: $(SRCS)
# don't generate deps  when cleaning
ifeq ($(findstring $(MAKECMDGOALS), clean distclean),)
	@for file in $(SRCS) ; do \
		$(CC) $(CFLAGS) -M $$file ; \
	done > dep.mk
endif

#  This file should only contain mandatory configuration info...
#clean:
#	@$(RM) -f *.o *.a *.jar *.ali *~ *.prtk dep.mk
