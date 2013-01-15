SOURCES = $(wildcard *.c)
TARGETS = $(basename $(SOURCES))

RM = rm -f
CFLAGS = -Wall -O2
CC = gcc

all: $(TARGETS) 

clean:
	@$(RM) *.o $(TARGETS) *~
distclean:
	@$(RM) *.o $(TARGETS) *~
