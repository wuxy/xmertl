# usage: include config.mk, from Makefiles
# this file contains all the .config configuration related things

# next variable should be set before including this file.
# In any case, if the makefile is located within the partikle tree 
# then we try to set it automatically.
ifndef PRTK
PRTK=$(shell echo `pwd`/ | sed -n \
       's|\(\(partikle[^/.]*/\)\+\).*|\1|p;' )
endif

$(if $(PRTK),, \
	$(warning "The configuration variable PRTK is not set") \
	$(error "edit this file to set the path to PaRTiKle directory."))

clean-targets := clean distclean
config-targets := config xconfig menuconfig oldconfig silentconfig help
.PHONY: $(clean-targets) $(config-targets) scripts_basic partikle

# skip .config when configuring
ifeq ($(findstring $(MAKECMDGOALS), $(config-targets) $(clean-targets)),)
need_config := 1
endif

# check if the .config exists
ifeq ($(PRTK)/.config, $(wildcard $(PRTK)/.config))
exists_config := 1
endif

# if there's no .config file warn the user and abort
$(if $(need_config), \
	$(if $(exists_config),, \
	$(warning "No .config file found at $(PRTK),") \
	$(error "run `make menuconfig` in the base directory")))

# if the .config is needed include it
ifdef exists_config
include $(PRTK)/.config

# If .config is newer than core/include/autoconf.h, someone tinkered
# with it and forgot to run make oldconfig.
$(PRTK)/core/include/autoconf.h: $(PRTK)/.config
	$(Q3)$(MAKE) -f $(PRTK)/Makefile silentoldconfig MAKEFLAGS=$(patsubst -j%,,$(MAKEFLAGS))

# set ARCH (architecture)
ifdef CONFIG_ARCH_X86
ARCH 		= i386
ARCH_STR	= "i386   "
endif

ifdef CONFIG_ARCH_ARM
ARCH 		= arm
ARCH_STR	= "arm    "
endif

ifdef CONFIG_ARCH_LINUX
ARCH 		= linux
ARCH_STR	= "linux  "
endif

ifdef CONFIG_ARCH_XTRATUM
ARCH 		= xm_i386
ARCH_STR	= "xm_i386"
endif

ifdef CONFIG_ARCH_LPC
ARCH 		= lpc
ARCH_STR	= "lpc    "
endif

supported-archs := i386 arm linux xm_i386 lpc
$(if $(findstring $(ARCH), $(supported-archs)),, \
	$(error "unknown architecture: ARCH=$(ARCH)"))
else
# Dummy target needed, because used as prerequisite
$(PRTK)/core/include/autoconf.h: ;
endif

# By default it is quiet. If "make V=1" then verbose.
quiet		= quiet_
Q		= echo -n ".";
Q2		=> /dev/null 2>&1
Q3		= @
MAKEFLAGS 	+= --no-print-directory --quiet

ifdef CONFIG_VERBOSE_COMPILE
	V=1
endif

ifdef V 
  ifeq ($(V),1)
    quiet 	=
    Q 		=
    Q2		=
    Q3		=
    MAKEFLAGS 	=
  endif
endif


include $(PRTK)/mkfiles/mkfile-chost

check_gcc = $(shell \
	if $(CC) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1 ;\
	then \
		echo "$(1)";\
	else \
		echo "$(2)";\
	fi)
