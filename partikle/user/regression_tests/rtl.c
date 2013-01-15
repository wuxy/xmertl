// A Hello World example

#include <rtl.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "regression.h"

int main (int argc, char **argv) {
  int cnt=0;
  struct timespec t = {5, 0};
  pthread_t self = pthread_self();
  
  
  if (pthread_setperiod_np(self, &t)){
    printf("pthread_setperiod_np failed\n");
    return TEST_FAIL;
  }

  pthread_make_periodic_np (pthread_self(), 0, 300200000LL);
  while (cnt++ < 5) {
	printf ("Hola Mundo!!!\n");
	pthread_wait_np();  //nanosleep (&t, 0);
  }
  return TEST_OK;
}
