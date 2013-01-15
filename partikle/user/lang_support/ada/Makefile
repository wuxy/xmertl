all: folders

folders:
	+$(Q)$(MAKE) -C GAP2005

distclean:
	$(Q)$(RM) -f *.o *.a *~
	+$(Q)$(MAKE) -C GAP2005 distclean

clean:
	$(Q)$(RM) -f *.o *.a *~
