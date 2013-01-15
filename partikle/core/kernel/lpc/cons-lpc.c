#include "lib.h"

// todo complete read_cons
int
read_cons(int fd, void * str, long n){ 
	return getstring_serialn(str, n);
}

int
write_cons(int fd, const void * str, long n){
	return putstring_serialn (str, n);
}

