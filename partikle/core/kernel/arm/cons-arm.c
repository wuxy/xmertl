#include "pl011.h"

// todo complete read_cons
int
read_cons(int fd, void * str, long n){ 
	return serial_getc();
}

int
write_cons(int fd, const void * str, long n){
	return serial_puts (str, n);
}

