#include "vga.h"
extern int keyb_read (char *str, unsigned long length);

int
read_cons(int fd, void * str, long n){
	return keyb_read (str, n);
}

int
write_cons(int fd, const void * str, long n){
	return  print_screen (str, n);
}

