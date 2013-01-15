CONF=prtkcf
LIBS=c
DEVS=\
	devcons.o\
	cons-i386.o\
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
	pit.o\
	keyb.o\

MISC=\
	irqs-i386.o\
	panic.o\
	pic.o\
	processor.o\
	stack.o\
	timers.o\
	user.o\
	vga.o\
	vmm.o\

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
all: links kernel.o
include ../../../config.mk
include ../../../core/rules.mk
include ../port/port.mk

ifdef CONFIG_PORT_DEVUART
DEVS += devuart.o
MISC += ../i386/uarti8250.o
endif

-include dep.mk
kernel.o: $(OBJS) $(LIBFILES)
	$(Q)$(LD) -r -o kernel.o $(OBJS) $(LIBOPTS)
clean:
	$(RM) *.o $(CONF).c $(CONF).root.c $(CONF).root.h

links:
	$(Q)if test -n "$(CONFIG_X86_PAGING)"; then \
		ln -sf partikle_mmu.lds partikle.lds; \
	else \
		ln -sf partikle_nommu.lds partikle.lds; \
	fi
