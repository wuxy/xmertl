SOURCES = $(wildcard *.c)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .prtk,$(NAMES))

PRTK=../../partikle

all: $(TARGETS)

include ../config.mk
include ${PRTK}/config.mk
include ${PRTK}/mkfiles/mkfile-c

clean:
	$(Q3)rm -f *.{prtk,dump,sym,size,hex,bin} *.ktr

