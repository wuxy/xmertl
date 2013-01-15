all: data2c

%: %.c
	$(Q)$(HOSTCC) $< -o $@

distclean: clean
	$(Q)$(RM) data2c

clean: 
	$(Q)$(RM) *.o *~
