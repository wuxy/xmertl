SRCS := $(sort $(wildcard *.c))
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
LOBJS := $(OBJS)

all: loader.xm

-include ../Rules.mk

../Rules.mk:
	make -C ../../ Rules.mk

-include dep.mk

ULIB_PATH=$(USER_TOOLS_PATH)/libxm

loader.xm: $(OBJS)
	@$(CC) -o loader.xm $(LOBJS) -L$(ULIB_PATH) -lxm
	@cp loader.xm unloader.xm

clean:
	$(RM) -f *.o *.a *~ loader.xm unloader.xm
	@find -name "*.o" -exec rm '{}' \;
	@find -name "*~" -exec rm '{}' \;
	@find -name "*.a" -exec rm '{}' \;
