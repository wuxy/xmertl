SOURCES = $(wildcard *.c)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .prtk,$(NAMES))

all: $(TARGETS)
include ../../../config.mk
include ../../../user/rules.mk

#-include dep.mk
%.bin: %.prtk
	$(Q3)$(OBJCOPY) -O binary --gap-fill 0 $< $@

%.prtk: %.o
	$(Q3)$(LDKERNEL) -vo $@  $<

%o: %c
	$(Q3)$(CC) $(CFLAGS) -c -o $@  $<

clean:
	$(Q3)rm -f *.prtk *.ktr
