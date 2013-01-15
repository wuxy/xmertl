SRCS := $(sort $(wildcard *.cc))
OBJS := $(patsubst %.cc,%.o,$(wildcard *.cc))
LOBJS := $(OBJS)

all: objs
include ../../../../config.mk
include ../../../../user/rules.mk

CFLAGS += -I../include -I../../include -I./ -fno-omit-frame-pointer
CFLAGS := $(shell echo $(CFLAGS) | sed 's/-Wno-pointer-sign//')

-include dep.mk

CXXFLAGS += -I../include -I../../include -I./ -fno-omit-frame-pointer
%.o: %.cc 
	$(CXX) $(CXXFLAGS) -c -o $@ $<

objs: $(OBJS)
	$(Q)$(AR) -r libsupc++.a $(LOBJS) $(Q2)
	$(Q)cp libsupc++.a ../../
