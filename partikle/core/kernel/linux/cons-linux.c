#include <arch/linux_syscalls.h>

int
read_cons(int fd, void * str, long n){
	return linux_read(0, (void *)str, n);
}

int
write_cons(int fd, const void * str, long n){
	return linux_write(1, str, n);
}

