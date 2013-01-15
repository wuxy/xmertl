.DEFAULT: all

all: 
	make -C ../ all

clean:
	@$(RM) -f *.o *~ *.ko
	@find -name "*.o" -exec rm '{}' \;
	@find -name "*~" -exec rm '{}' \;
