#include <arch/xm_syscalls.h>

int
read_cons(int fd, void * str, long n){
	return 0;
}

int
write_cons(int fd, const void * str, long n){
	return xm_write_scr ((char *)str, n);
}

