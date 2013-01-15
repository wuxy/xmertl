SRCS := $(sort $(wildcard *.c *.S))
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
OBJS := $(OBJS) $(patsubst %.S,%.o,$(wildcard *.S))
LOBJS := $(OBJS)

all: objs

-include ../../Rules.mk

../../Rules.mk:
	make -C ../../../ Rules.mk

ULIB_PATH=$(USER_TOOLS_PATH)/libxm

-include dep.mk

objs: $(OBJS)
	@$(AR) -r libxm.a $(LOBJS)
	@$(AR) -d libxm.a start.o
	@cp libxm.a start.o ../

clean:
	@$(RM) -f *.o *.a *~
	@find -name "dep.mk" -exec rm '{}' \;
	@find -name "*.o" -exec rm '{}' \;
	@find -name "*~" -exec rm '{}' \;
	@find -name "*.a" -exec rm '{}' \;
