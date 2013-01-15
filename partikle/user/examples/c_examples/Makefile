SOURCES = $(wildcard *.c)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .prtk,$(NAMES))

all: $(TARGETS)
include ../../../config.mk
# warning: adds CFLAGS to the env, making pgcc/ldkernel fail!
#include ../../../mkfiles/mkfile-$(ARCH)
include ../../../mkfiles/mkfile-c

clean:
	$(Q3)rm -f *.{prtk,dump,sym,size,hex,bin} *.ktr

