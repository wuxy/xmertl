// A Hello World example

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <sys/io.h>

int main (int argc, char **argv) {
  int err;

  printf("error message list:\n");
  for (err=0; err<EKEYREJECTED; err++)
	  printf("errno %d: \"%s\"\n", err, strerror(err));
  return 0;
}
