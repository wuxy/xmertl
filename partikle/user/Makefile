all: default 
include ../config.mk
include ../user/rules.mk

default: bin/Config.scr
	+$(Q)$(MAKE) -C ulibc $(MAKECMDGOALS);
	+$(Q)$(MAKE) -C lang_support $(MAKECMDGOALS);
	+$(Q)$(MAKE) -C bin

bin/Config.scr: $(PRTK)/.config
	$(Q)( \
	echo "# This file has been created automatically";	\
	echo "# Please, don't change anything";				\
	echo "PRTK=$(PRTK)";								\
	echo "ARCH=$(ARCH)";								\
	echo "CONFIG_CXX_SUPPORT=$(CONFIG_CXX_SUPPORT)";	\
	echo "CONFIG_ADA_SUPPORT=$(CONFIG_ADA_SUPPORT)";	\
	echo "GNAT_PATH=$(GNAT_PATH)"; 						\
	echo "CONFIG_JAVA_SUPPORT=$(CONFIG_JAVA_SUPPORT)";	\
	echo "DEBFLAGS=-S";									\
	echo "LIBGCC=\`$(CC) -print-libgcc-file-name\`";	\
	echo "CFLAGS='$(CFLAGS)'";	\
	echo "CC='$(CC)'";									\
	echo "LD='$(LD)'";									\
	echo "OBJCOPY=$(OBJCOPY)"; ) > bin/Config.scr

distclean clean:
	$(Q)$(RM) -f *.o *.a *.jar *~ bin/Config.scr
	+$(Q)$(MAKE) -C lang_support $(MAKECMDGOALS)
	+$(Q)$(MAKE) -C examples $(MAKECMDGOALS)
	+$(Q)$(MAKE) -C ulibc $(MAKECMDGOALS)
	+$(Q)$(MAKE) -C bin $(MAKECMDGOALS)
	#$(Q)$(MAKE) -C posixtestsuite distclean
	

