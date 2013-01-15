SOURCES = $(wildcard *.c)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .xmd,$(NAMES))

MKDOMAIN=../../scripts/mkdomain

all: $(TARGETS)

-include ../../Rules.mk


../../Rules.mk:
	make -C ../../ Rules.mk


%.xmd: %.o
	$(MKDOMAIN) -o $@  $<

%o: %c
	$(CC) $(CFLAGS) -c -o $@  $<

clean:
	$(RM) -f *~ *.o *.xmd
