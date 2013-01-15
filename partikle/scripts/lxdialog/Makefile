HOSTCFLAGS += -DLOCALE 
LIBS = -lncurses

ifeq (/usr/include/ncurses/ncurses.h, $(wildcard /usr/include/ncurses/ncurses.h))
        HOSTCFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses.h>"
else
ifeq (/usr/include/ncurses/curses.h, $(wildcard /usr/include/ncurses/curses.h))
        HOSTCFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses/curses.h>"
else
ifeq (/usr/include/ncurses.h, $(wildcard /usr/include/ncurses.h))
        HOSTCFLAGS += -DCURSES_LOC="<ncurses.h>"
else
	HOSTCFLAGS += -DCURSES_LOC="<curses.h>"
endif
endif
endif


OBJS = checklist.o menubox.o textbox.o yesno.o inputbox.o \
       util.o lxdialog.o msgbox.o

%.o: %.c
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<

all: ncurses lxdialog

lxdialog: $(OBJS)
	$(Q)$(HOSTCC) -o lxdialog $(OBJS) $(LIBS)

ncurses:
	@echo "main() {}" > lxtemp.c
	@if $(HOSTCC) -lncurses lxtemp.c ; then \
		rm -f lxtemp.c a.out; \
	else \
		rm -f lxtemp.c; \
		echo -e "\007" ;\
		echo ">> Unable to find the Ncurses libraries." ;\
		echo ">>" ;\
		echo ">> You must have Ncurses installed in order to use 'make menuconfig'" ;\
		echo ">> If you are using Debian, or a Debian dereived distribution, then " ;\
		echo ">> the name of the package is: libncurses5-dev" ;\
		echo ;\
		exit 1 ;\
	fi

clean:
	$(Q)rm -f core *.o *~ lxdialog

distclean:
	$(Q)rm -f core *.o *~ lxdialog

