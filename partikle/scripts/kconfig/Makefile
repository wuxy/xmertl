# ===========================================================================
# lxdialog only configuration utility.
# 


conf-objs	:= conf.o  zconf.tab.o
mconf-objs	:= mconf.o zconf.tab.o
qconf-cxxobjs   := qconf.o
qconf-objs      := kconfig_load.o zconf.tab.o

# generated files seem to need this to find local include files
HOSTLOADLIBES_qconf	= $(KC_QT_LIBS) -ldl
HOSTCXXFLAGS_qconf.o	= $(KC_QT_CFLAGS) -D LKC_DIRECT_LINK

all: mconf conf

%.o: %.c
	$(Q3)$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<

mconf: $(mconf-objs)
	$(Q3)$(HOSTCC) -o mconf $(mconf-objs)

conf: $(conf-objs)
	$(Q3)$(HOSTCC) -o conf $(conf-objs)

qconf: .tmp_qtcheck $(qconf-objs)
	$(Q3)$(HOSTCXX) -o qconf $(qconf-objs)

# QT needs some extra effort...
.tmp_qtcheck:
	@set -e; echo "  CHECK   qt"; dir=""; pkg=""; \
	pkg-config --exists qt 2> /dev/null && pkg=qt; \
	pkg-config --exists qt-mt 2> /dev/null && pkg=qt-mt; \
	if [ -n "$$pkg" ]; then \
	  cflags="\$$(shell pkg-config $$pkg --cflags)"; \
	  libs="\$$(shell pkg-config $$pkg --libs)"; \
	  moc="\$$(shell pkg-config $$pkg --variable=prefix)/bin/moc"; \
	  dir="$$(pkg-config $$pkg --variable=prefix)"; \
	else \
	  for d in $$QTDIR /usr/share/qt* /usr/lib/qt*; do \
	    if [ -f $$d/include/qconfig.h ]; then dir=$$d; break; fi; \
	    if [ -f $$d/libQtCore.la ]; then dir=$$d; break; fi; \
	  done; \
	  if [ -z "$$dir" ]; then \
	    echo "*"; \
	    echo "* Unable to find the QT installation. Please make sure that"; \
	    echo "* the QT development package is correctly installed and"; \
	    echo "* either install pkg-config or set the QTDIR environment"; \
	    echo "* variable to the correct location."; \
	    echo "*"; \
	    false; \
	  fi; \
	  libpath=$$dir/lib; lib=qt; osdir=""; \
	  $(HOSTCXX) -print-multi-os-directory > /dev/null 2>&1 && \
	    osdir=x$$($(HOSTCXX) -print-multi-os-directory); \
	  test -d $$libpath/$$osdir && libpath=$$libpath/$$osdir; \
	  test -f $$libpath/libqt-mt.so && lib=qt-mt; \
	  cflags="-I$$dir/include"; \
	  libs="-L$$libpath -Wl,-rpath,$$libpath -l$$lib"; \
	  moc="$$dir/bin/moc"; \
	fi; \
	if [ ! -x $$dir/bin/moc -a -x /usr/bin/moc ]; then \
	  echo "*"; \
	  echo "* Unable to find $$dir/bin/moc, using /usr/bin/moc instead."; \
	  echo "*"; \
	  moc="/usr/bin/moc"; \
	fi; \
	echo "KC_QT_CFLAGS=$$cflags" > $@; \
	echo "KC_QT_LIBS=$$libs" >> $@; \
	echo "KC_QT_MOC=$$moc" >> $@

kconfig_load.o: lkc_defs.h
qconf.o: qconf.moc lkc_defs.h

%.moc: %.h
	$(KC_QT_MOC) -i $< -o $@

lkc_defs.h: lkc_proto.h
	sed < $< > $@ 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

clean:
	$(Q)rm -f core *.o *~ mconf conf .dep .tmp_qtcheck

distclean:
	$(Q)rm -f core *.o *~ mconf conf .dep .tmp_qtcheck

dep.mk:
# don't generate deps  when cleaning
ifeq ($(findstring $(MAKECMDGOALS), clean distclean),)
	@for file in $(patsubst %.o,%.c,$(OBJS)) ; do \
		$(Q)$(CC) $(CFLAGS) -M $$file ; \
	done > dep.mk
endif

-include dep.mk
