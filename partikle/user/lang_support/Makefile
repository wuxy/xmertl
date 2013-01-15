all: folders
include ../../config.mk
include ../../user/rules.mk

links:
	$(Q)if [ ! -d $(USER_PATH)/lang_support/include/arch ] ; then \
		ln -sf $(USER_PATH)/lang_support/include/$(ARCH) $(USER_PATH)/lang_support/include/arch; \
	fi

folders: links
#	@exec echo -e "\n>> Building languages support:"
#	@exec echo -n " > DWARF2 support: "
	+$(Q)$(MAKE) -C gcc all
ifdef CONFIG_CXX_SUPPORT
#	@exec echo -n " > C++ support: "
	+$(Q)$(MAKE) -C cxx all
#	@exec echo ""
endif
ifdef CONFIG_JAVA_SUPPORT
#	@exec echo -n " > Java support: "
	+$(Q)$(MAKE) -C java all
#	@exec echo ""
endif
ifdef CONFIG_ADA_SUPPORT
#	@exec echo -n " > Ada support: "
	+$(Q)$(MAKE) -C ada all
#	@exec echo ""
endif

distclean: clean
clean:
	$(Q)$(RM) -f *.o *.a *.jar *~
	+$(Q)$(MAKE) -C gcc $@
ifdef CONFIG_CXX_SUPPORT
	+$(Q)$(MAKE) -C cxx $@
endif
ifdef CONFIG_JAVA_SUPPORT
	+$(Q)$(MAKE) -C java $@
endif
	+$(Q)$(MAKE) -C ada $@
	$(Q)$(RM) -f $(USER_PATH)/lang_support/include/arch
