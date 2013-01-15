AWK = awk
O = o
DATA2C = $(PRTK)/scripts/utils/data2c
export CONF PRTK AWK O DATA2C

KERNDATE:=$(shell date '+%s')
CFLAGS += -I../port -DKERNDATE=$(KERNDATE)
ASFLAGS += -D KERNELRELEASE='"'$(KERNELRELEASE)'"'  -D ARCH_STR='"'$(ARCH_STR)'"'

LIBOPTS= $(shell echo $(LIBS) | sed 's|\([a-z]*\)|-L'$(PRTK)/core/lib'\1 -l\1|g')
LIBFILES=$(shell echo $(LIBS) | sed 's| |\n|' | sed 's|\(.*\)|$(PRTK)/core/lib\1/lib\1.a|g')

# this rule matches $(PRTK)/core/lib%/%.a to build lib%.a,
# because make only performs first replacement of % in the rule target
%.a: 
	+(LIBDIR=$(shell dirname $@); \
	if test -d $$LIBDIR; then \
		make -C $$LIBDIR all; \
	else \
		echo error: unexistent lib: $$LIBDIR; \
		exit 1; \
	fi)

$(CONF).c:	../port/mkdevb $(CONF)
	(echo dev  $(DEVS); \
	 echo link $(LINK)) | sh ../port/mkdevb > $(CONF).c
$(CONF).root.h: $(CONF) ../port/mkroot
	sh ../port/mkroot -h $(CONF)
$(CONF).root.c: $(CONF) ../port/mkroot
	sh ../port/mkroot -c $(CONF)
$(CONF).$O: $(CONF).root.h

# to reach source from ../port
# rule order is important as first fired wins
%.o: ../port/%.c
	$(CC) $(CFLAGS) $< -c -o $@
%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@
%.o: %.S
	$(CC) $(ASFLAGS) $< -c -o $@
%.o: ../port/%.S
	$(CC) $(ASFLAGS) $< -c -o $@
