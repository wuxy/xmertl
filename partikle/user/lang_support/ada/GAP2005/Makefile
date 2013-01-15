all: objs

RTS_OBJS=a-filico.adb g-io.adb i-c.adb s-arit64.adb s-imgenu.adb s-imgint.adb \
	s-imguns.adb s-strops.adb s-sopco5.adb s-addima.adb s-io.adb \
	s-tassta.adb s-soflin.adb s-imgrea.adb

SRCS := $(sort $(wildcard *.c))
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

ADAOBJS := $(patsubst %.ali,%.o,$(wildcard *.ali))

include ../../../../config.mk
include ../../../../user/rules.mk

-include dep.mk

GNATMAKE=$(GNAT_PATH)/bin/gnatmake
RTS_PATH=`$(GNAT_PATH)/bin/gnatls --RTS=sjlj -v | awk '/rts-sjlj\/adainclude/{printf("%s", $$1)}'`
GNATMAKEFLAGS=-q -O2 -a -I$(PRTK)/lang_support/ada/GAP2005/ --RTS=rts-sjlj

objs: $(OBJS)
	# compilation of pgnat.ads warnings are ignored, to see them use V=1
	$(Q)$(GNATMAKE) $(GNATMAKEFLAGS) -gnatf -gnatwl pgnat.ads 2>&1 |\
		if test -z $(V); then sed '/warning:/d'; else cat; fi

	$(Q)for file in $(RTS_OBJS); do \
		$(GNATMAKE) -a -q -O2 $(RTS_PATH)/$$file ; \
	done	
	$(Q)$(AR) -q ../../libpgnat.a $(OBJS) $(ADAOBJS) $(Q2)

distclean:
	$(Q)$(RM) -f *.o *.a *~ *.ali
