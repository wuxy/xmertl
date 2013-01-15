SOURCES = $(wildcard *.c string/*.c)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .prtk,$(NAMES))

all: $(TARGETS)
include ../../config.mk
include ../../mkfiles/mkfile-c

tests: $(TARGETS)
# run all the targets until one fails
	$(Q)for t in $(TARGETS); do		\
		if ! ./$$t; then		\
			echo $$t: test failed;	\
		fi				\
	done
	exit 0

#-include dep.mk

clean:
	$(Q3)rm -f *.prtk *.ktr
	+$(Q3)$(MAKE) -C string $(MAKECMDGOALS)
