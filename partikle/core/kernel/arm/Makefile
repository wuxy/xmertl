CONF= prtkcf
LIBS=c
DEVS=\
	devcons.o\
	cons-arm.o\
	devroot.o\
	devtest.o\

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
	icp_pit.o\
	
MISC=\
	icp_pic.o\
	irqs-arm.o\
	panic.o\
	processor.o\
	pl011.o\
	stack.o\
	sysmem.o\
	timers.o\
	user.o\

ROOT=\
	$(CONF).o\
	$(CONF).root.o\

OBJS=\
	entry.o\
	head.o\
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

-include dep.mk
kernel.o: $(OBJS) $(LIBFILES)
	$(Q)$(LD) -r -o kernel.o $(OBJS) $(LIBOPTS)
clean:
	$(RM) *.o $(CONF).c $(CONF).root.c $(CONF).root.h

