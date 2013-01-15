/*
 * A Hello World example
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

pthread_t th_id;
void *foo (void *);

int main (int argc, char **argv) {
  struct timespec t = {0, 500000000LL};
  int counter;

  for (counter=0; counter < 10; counter++) { 
    printf ("Hello ");
    
    pthread_create(&th_id, NULL, foo ,"World!!\n");
    pthread_join(th_id, NULL);

    nanosleep (&t, 0);

  }
  return 0;
}
