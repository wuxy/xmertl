CONF= prtkcf
LIBS=rc # reduced libc
DEVS=\
	devcons.o cons-lpc.o\

PORT=\
	dev.o\
	errno.o\
	fs.o\
	heap.o\
	irqs.o\
	pthread.o\
	sched.o\
	setup.o\
	signal.o\
	syscalls.o\
	time.o\
	rtl.o\
	semaphore.o\

LINK=\
	koutput.o\
	rtc.o\
	timers.o\
	
MISC=\
	trap.o\
	uart.o\
	\
	processor.o\
	stack.o\
	sysmem.o\
	user.o\
	lib.o

SRCS= $(wildcard $(patsubst %.o, %.c, $(OBJS)))

ROOT=\
	$(CONF).o\
	$(CONF).root.o\

OBJS=\
	boot.o\
	\
	$(PORT)\
	$(DEVS)\
	$(LINK)\
	$(MISC)\
	$(ROOT)\

.SUFFIXES:
all: kernel.o # links
include ../../../config.mk
include ../../../core/rules.mk
include ../port/port.mk

-include dep.mk

kernel.o: $(OBJS) $(LIBFILES)
	$(Q)$(LD) $(LDFLAGS) -r -o kernel.o $(OBJS) $(LIBOPTS)
clean:
	$(RM) *.o $(CONF).c $(CONF).root.c $(CONF).root.h
