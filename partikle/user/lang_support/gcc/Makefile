SRCS := $(sort $(wildcard *.c))
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
LOBJS := $(OBJS)

all: objs
include ../../../config.mk
include ../../../user/rules.mk

CFLAGS += -I../include

#-Dinhibit_libc  -DIN_GCC \
	  #-static -I../include -I./	

-include dep.mk

#CFLAGS = $LAGS) -finhibit-size-directive \
        -fno-inline-functions -fno-exceptions -fPIC

#CFLAGS += $(call check_gcc,-fno-zero-initialized-in-bss,)

objs: $(OBJS)
	$(Q)$(AR) -r libgcc_eh.a $(LOBJS) $(Q2)

distclean: clean
clean:
	$(Q)$(RM) *.o *.a
