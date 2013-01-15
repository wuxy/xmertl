all: |links partikle_core.o
include ../config.mk
include ../core/rules.mk
.PHONY: all partikle_core.o
links:
	$(Q)if [ ! -d $(PRTK)/core/include/arch ] ; then \
		ln -sf $(PRTK)/core/include/$(ARCH) $(PRTK)/core/include/arch; \
	fi

partikle_core.o: 
	+$(Q)$(MAKE) -C kernel/$(ARCH) all
	$(Q)$(LD) -r -o partikle_core.o kernel/$(ARCH)/kernel.o $(LIBGCC)
	$(Q)$(OBJCOPY) -G _start -G syscall_gate partikle_core.o


# Clean all the folders even, those of other archs.
distclean:
	$(Q)$(RM) *.o *~
	+$(Q)$(MAKE) -C libc clean	
	$(RM) -f libs/*.a
	+$(Q)for i in  kernel/* ; do \
		if [ -f  $$i/Makefile  ]; then \
			$(MAKE) -C $$i clean ; \
	        fi;\
	done
clean:
	$(Q)$(RM) *.o *~
	$(RM) -f libs/*.a
	+$(Q)$(MAKE) -C libc clean
ifdef ARCH
	+$(Q)$(MAKE) -C kernel/$(ARCH) clean
endif

