all: folders

folders:
	+$(Q)$(MAKE) -C libsupc++

distclean:
	$(Q)$(RM) -f *.o *.a *~
	+$(Q)$(MAKE) -C libsupc++ clean

clean:
	$(Q)$(RM) -f *.o *.a *~
