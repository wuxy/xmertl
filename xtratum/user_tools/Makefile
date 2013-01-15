all: folders

-include Rules.mk

Rules.mk:
	@make -C ../ Rules.mk

folders: links
	@make -C libxm all
	@make -C xmloader all

ULIB_PATH=$(USER_TOOLS_PATH)/libxm

links:
	@if [ ! -d $(ULIB_PATH)/include/arch ] ; then \
		ln -sf $(ULIB_PATH)/include/$(XM_ARCH) $(ULIB_PATH)/include/arch; \
	fi
	@if [ ! -d $(ULIB_PATH)/include/xm ] ; then \
		ln -sf $(XM_PATH)/include $(ULIB_PATH)/include/xm; \
	fi

clean:
	@$(RM) -f Rules.mk $(ULIB_PATH)/include/arch $(ULIB_PATH)/include/xm
	@find -name "*.o" -exec rm '{}' \;
	@find -name "*~" -exec rm '{}' \;
	@find -name "*.a" -exec rm '{}' \;
	@find -name "dep.mk" -exec rm '{}' \;
