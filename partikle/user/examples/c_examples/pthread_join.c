// Creating our first thread

#include <stdio.h>
#include <time.h>
#include <pthread.h>


static pthread_t t1;

//static int c1 = 0;

void *f(void *args) {
  struct timespec t = {2, 0};

  /*while (1) {
    printf ("(%d) Soy t1: %p\n", c1 ++,   pthread_self ());
    nanosleep (&t, 0);
    }*/
  
  nanosleep (&t, 0);

  //pthread_detach (t1);

  return (void *)23;
}

int main (int argc, char **argv) {
  int ret_v;

  pthread_create (&t1, 0, f, 0);

  pthread_join (t1, (void *) &ret_v);

  printf ("Returned value: %d\n", ret_v);

  pthread_exit ((void *)0);
  return 0;
}
