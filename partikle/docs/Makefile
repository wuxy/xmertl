SUBDIRS= usermanual

%: force
	+@for d in $(SUBDIRS); do \
		cd $$d; make $(*); \
	done
force: ;
