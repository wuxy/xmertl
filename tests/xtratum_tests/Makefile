SOURCES = $(wildcard *.c)
NAMES = $(basename $(SOURCES))
TARGETS = $(addsuffix .xmd,$(NAMES))

USER_TOOLS_PATH=${XMERTL_ROOT}/xtratum/user_tools
MKDOMAIN=${USER_TOOLS_PATH}/scripts/mkdomain

all: $(TARGETS)

include ../config.mk
include ${USER_TOOLS_PATH}/Rules.mk


${USER_TOOLS_PATH}/Rules.mk:
	make -C ${USER_TOOLS_PATH}/Rules.mk


%.xmd: %.o
	$(MKDOMAIN) -o $@  $<

%o: %c
	$(CC) $(CFLAGS) -c -o $@  $<

clean:
	$(RM) -f *~ *.o *.xmd
