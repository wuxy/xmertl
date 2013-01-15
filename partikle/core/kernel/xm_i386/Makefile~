CONF=prtkcf
LIBS=c
DEVS=\
	devcons.o\
	cons-xm.o\
	devroot.o\

PORT=\
	dev.o\
	errno.o\
	fs.o\
	heap.o\
	irqs.o\
	pthread.o\
	rtl.o\
	sched.o\
	semaphore.o\
	setup.o\
	signal.o\
	syscalls.o\
	time.o\
	note.o\

LINK=\
	koutput.o\
	rtc.o\
	tsc.o\
	xmtimer.o\
	
MISC=\
	irqs_xm.o\
	panic.o\
	processor.o\
	stack.o\
	sysmem.o\
	timers.o\
	user.o\
	xmsyscalls.o\

ROOT=\
	$(CONF).o\
	$(CONF).root.o\

OBJS=\
	start.o\
	syscall_gate.o\
	\
	$(PORT)\
	$(DEVS)\
	$(LINK)\
	$(MISC)\
	$(ROOT)\

SRCS= $(wildcard $(patsubst %.o, %.c, $(OBJS)))

.SUFFIXES:
all: kernel.o
include ../../../config.mk
include ../../../core/rules.mk
include ../port/port.mk

ifdef CONFIG_XTRATUM_FIFO
DEVS += devrtf.o
endif

ifdef CONFIG_PORT_DEVUART
DEVS += devuart.o
MISC += ../i386/uarti8250.o
endif

-include dep.mk
kernel.o: $(OBJS) $(LIBFILES)
	$(Q)$(LD) -r -o kernel.o $(OBJS) $(LIBOPTS)
clean:
	$(RM) *.o $(CONF).c $(CONF).root.c $(CONF).root.h

