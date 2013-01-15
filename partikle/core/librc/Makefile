SRCS := $(sort $(wildcard *.c *.S))

TARG=\
	librc.a\

OBJS=\
	rprintf.o\
	string.o\
	time.o\
	tlsf.o\

all: $(TARG)
include ../../config.mk
include ../../core/rules.mk

ifeq ($(ARCH),arm)
SRCS := $(shell echo $(SRCS) | sed 's/log.S//')
endif
ifeq ($(ARCH),lpc)
SRCS := $(shell echo $(SRCS) | sed 's/log.S//')
endif

-include dep.mk

$(TARG): $(OBJS)
	$(Q)ar rus $@ $(OBJS) $(Q2)
clean:
	$(Q)$(RM) $(TARG) $(OBJS)
	$(RM) *.o *.a
