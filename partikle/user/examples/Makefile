all: folders
include ../../config.mk
include ../../user/rules.mk

folders:
	+$(Q)$(MAKE) -C c_examples
ifdef CONFIG_CXX_SUPPORT
	+$(Q)$(MAKE) -C cxx_examples
endif
ifdef CONFIG_ADA_SUPPORT
	+$(Q)$(MAKE) -C ada_examples
endif

distclean clean:
	$(Q)$(RM) -f *.o *.a *~
	+$(Q)$(MAKE) -C c_examples $(MAKECMDGOALS)
	+$(Q)$(MAKE) -C cxx_examples $(MAKECMDGOALS)
#ifdef CONFIG_JAVA_SUPPORT
#	+$(Q)make -C java_examples clean
#endif
ifdef CONFIG_ADA_SUPPORT
	+$(Q)$(MAKE) -C ada_examples clean
endif
