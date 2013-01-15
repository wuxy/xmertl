#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "regression.h"

#define NELEMS 10

int myglobal = 0;
sem_t sem;
const struct timespec tr = {0,100000000};

void *thread_function(void *arg) {
  int i,j;
  for ( i=0; i<NELEMS; i++ ) {
    sem_wait(&sem);
    j=myglobal;
    j=j-1;
    printf(".");
    nanosleep(&tr, 0);
    myglobal=j;
    sem_post(&sem);
  }
  return NULL;
}

int main(void) {
  pthread_t mythread;
  int i;
  
  sem_init(&sem, 0, 1);
  if ( pthread_create( &mythread, NULL, thread_function, NULL) ) {
    printf("error creating thread.");
    abort();
  }
  for ( i=0; i<NELEMS; i++) {
    sem_wait(&sem);
    myglobal=myglobal+1;
    sem_post(&sem);
    printf("o");
    nanosleep(&tr, 0);
  }
  if (pthread_join (mythread, NULL)) {
    printf("error joining thread.");
    abort();
  }
  printf("\nmyglobal equals %d\n",myglobal);

  if (myglobal != 0)
    return TEST_FAIL;
  return TEST_OK;
}
