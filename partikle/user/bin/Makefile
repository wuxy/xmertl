all: pkver
include ../../config.mk

pkver: pkver.c
	$(Q3)$(HOSTCC) -o pkver pkver.c

distclean clean:
	rm -f pkver *.o *~

.PHONY: clean  distclean
