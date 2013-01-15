all: default
include ../../config.mk
include ../../user/rules.mk

default:
	$(Q)if [ ! -d $(ULIB_PATH)/include/arch ] ; then \
		ln -sf $(ULIB_PATH)/include/$(ARCH) $(ULIB_PATH)/include/arch; \
	fi
	$(Q)if [ ! -d $(ULIB_PATH)/include/kernel_inc ] ; then \
		ln -sf $(PRTK)/core/include $(ULIB_PATH)/include/kernel_inc; \
	fi
	+$(Q)$(MAKE) -C libc/ all

clean:
	+$(Q)$(MAKE) -C libc clean

distclean:
	+$(Q)$(MAKE) -C libc $@
	$(Q)rm -f $(ULIB_PATH)/include/arch $(ULIB_PATH)/include/kernel_inc
