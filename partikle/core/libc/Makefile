SRCS := $(sort $(wildcard *.c *.S))

OBJS=\
	stdarg.o\
	stdio.o\
	stdlib.o\
	string.o\
	time.o\
	tlsf.o\
	vfprintf.o\
	vsprintf.o\

all: libc.a
include ../../config.mk
include ../../core/rules.mk

ifneq ($(findstring $(ARCH), i386 linux xm_i386),)
SRCS += log.S
OBJS += log.o
endif

-include dep.mk

libc.a: $(OBJS)
	$(Q)ar rus $@ $(OBJS) $(Q2)
clean:
	$(Q)$(RM) libc.a $(OBJS)
