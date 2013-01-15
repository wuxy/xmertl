# usage: include rules.mk, from Makefiles
# this file contains all common definitions related/needed by userland

USER_PATH = $(PRTK)/user
ULIB_PATH = $(USER_PATH)/ulibc
SCRIPTS_PATH=$(USER_PATH)/bin
ULIBC=$(ULIB_PATH)/ulib.a
LDKERNEL=$(USER_PATH)/bin/ldkernel 
GNAT_PATH	= $(CONFIG_GNAT_PATH)
GNATVERSION	= GAP2005

CFLAGS=-fexceptions -Wall -O2 -fno-builtin -nostdlib -nostdinc -D$(ARCH) -I$(ULIB_PATH)/include -fno-strict-aliasing

CXXFLAGS=-fexceptions -Wall -O2 -fno-builtin -nostdlib -nostdinc -D$(ARCH) -I$(ULIB_PATH)/include -fno-strict-aliasing

GCJFLAGS=-fexceptions -Wswitch-enum -ffloat-store -Wno-deprecated --encoding=UTF-8 -nostdinc -nostdinc++ -O2 -I$(ULIB_PATH)/include -I$(USER_PATH)/lang_support/java/libgcj/include -I$(USER_PATH)/lang_support/java/libgcj/include_class

# set up compiler toolchain
ifdef ARCH
include $(PRTK)/mkfiles/mkfile-$(ARCH)
endif

CFLAGS += $(call check_gcc, -Wno-pointer-sign,)
CFLAGS += $(call check_gcc, -fno-stack-protector,)

ifdef CONFIG_DEBUGGING
	CFLAGS+=-g
	CXXFLAGS+=-g
	GCJFLAGS+=-g
else
	CFLAGS+=-fomit-frame-pointer
	CXXFLAGS+=-fomit-frame-pointer
	GCJFLAGS+=-fomit-frame-pointer
endif

ASFLAGS = -Wall -O2 -D__ASSEMBLY__ -fno-builtin  -D$(ARCH) -I$(ULIB_PATH)/include -nostdlib -nostdinc 

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
