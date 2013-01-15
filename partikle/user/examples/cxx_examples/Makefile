SOURCES = $(wildcard *.cc)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .prtk,$(NAMES))

all: $(TARGETS)
include ../../../config.mk
include ../../../mkfiles/mkfile-cxx

clean:
	$(Q3)rm -f *.prtk *.ktr
