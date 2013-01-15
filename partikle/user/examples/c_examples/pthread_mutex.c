// Creating our first thread

#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

pthread_t t1, t2;
pthread_mutex_t m1;// = PTHREAD_MUTEX_INITIALIZER;

int c1 = 0, c2 = 0;
void *f2(void *args) {
  //struct timespec t = {2, 0};
  pthread_mutex_lock (&m1);
  printf ("(%d) Soy t2: %p\n", c2++, pthread_self ());
  
  /*while (1) {
    printf ("(%d) Soy t2: %p\n", c2++, pthread_self ());
    nanosleep (&t, 0);
    }*/
  return (void *)0;
}

void *f(void *args) {
  struct timespec t = {2, 0};
  //  pthread_create (&t2, 0, f2, 0);
  pthread_mutex_lock (&m1);
  pthread_mutex_trylock (&m1);
  printf ("(%d) Soy t1: %p\n", c1 ++,   pthread_self ());
  nanosleep (&t, 0);
  pthread_mutex_unlock (&m1);
  /*while (1) {
    printf ("(%d) Soy t1: %p\n", c1 ++,   pthread_self ());
    nanosleep (&t, 0);
    }*/

  return (void *)0;
}

int main (int argc, char **argv) {
  pthread_mutexattr_t at;
  pthread_mutexattr_init (&at);
  //pthread_mutexattr_settype (&at, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init (&m1, &at);
  pthread_create (&t1, 0, f, 0);
  pthread_create (&t2, 0, f2, 0);
  
  pthread_join (t1, NULL);
  pthread_join (t2, NULL);
  return 0;
}
