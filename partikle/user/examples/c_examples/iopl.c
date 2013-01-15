// A Hello World example

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/io.h>

int main (int argc, char **argv) {
  struct timespec t = {1, 0};
  int ret, port =0x378;
  printf ("provide io port: ");
  scanf ("%d", &port);
  printf ("port: %d\n", port);

  if ((ret=iopl(3)) < 0){
	printf("iopl: ret %d errno %d %s\n", ret, errno, strerror(errno));
	exit (-1);
  }

  while (1){
	printf ("%d = inb (%d)\n", inb (port), port);
	nanosleep(&t,0);
  }
  return 0;
}
