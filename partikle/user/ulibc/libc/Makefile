SRCS = $(sort $(wildcard *.c))
OBJS = $(shell echo $(SRCS) | sed 's|\.[cS]|.o|g')

all: ulibc.a
include ../../../config.mk
include ../../../user/rules.mk
-include dep.mk

SRCS += $(wildcard $(ARCH)/*.c $(ARCH)/*.S)
ifeq ($(findstring $(ARCH), linux xm_i386),$(ARCH))
SRCS += $(wildcard i386/*.S)
endif

# uncomment to update the error messages from errno.h
#errstr.c: ../../../core/include/errno.h $(PRTK)/.config
#	$(Q) (	\
#	echo;	\
#	echo '/* file automatically generated, do not edit */';	\
#	echo '#include <config.h>';	\
#	echo '#include <errno.h>';	\
#	echo '#ifdef CONFIG_ULIBC_ERRSTR';	\
#	echo;	\
#	echo 'const char *errstr[__SYS_NERR] = {';	\
#	echo '	[0]	=	"Success",'; \
#	sed -n '/#define[ \t]*E/ s|#define[ \t]*\([A-Z1-9]\+\).*/\* \(.*\) \*/|\t[\1] =\t"\2",|p' $<;	\
#	echo '};';	\
#	echo;	\
#	echo '#else';	\
#	echo 'const char * errstr[2] = {';	\
#	echo '	[0]	=	"Success",'; \
#	echo '	"errstr list disabled at config time"';	\
#	echo '};';	\
#	echo '#endif';	\
#	echo;	\
#	echo 'const char  __sys_err_unknown [] = "[unknown error]";';	\
#	) > $@

ulibc.a: $(OBJS)
	$(Q)$(AR) -r ulibc.a $(OBJS) $(Q2)
	$(Q)$(AR) -d ulibc.a user_entry_point.o $(Q2)

distclean clean:
	$(Q)$(RM) -f ulibc.a $(OBJS) *.o dep.mk
