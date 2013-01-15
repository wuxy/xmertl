all: 	tracer

tracer: tracer.c tracedump.h
	$(CC) $(INCLUDE)  -I../include  -O2 -Wall -o tracer tracer.c

clean:
	rm -f tracer *.o *.ko

