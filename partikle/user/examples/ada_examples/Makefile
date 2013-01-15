SOURCES = $(wildcard *.adb)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .prtk,$(NAMES))

all: $(TARGETS)
include ../../../config.mk
include ../../../mkfiles/mkfile-ada

clean:
	$(Q3)rm -f $(NAMES) $(TARGETS) $(SRCS:.adb:) *.o *.ali *.ktr
