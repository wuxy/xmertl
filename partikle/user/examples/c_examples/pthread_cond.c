// Creating our first thread

#include <stdio.h>
#include <time.h>
#include <pthread.h>

pthread_t t1, t2;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

int c1 = 0, c2 = 0;
void *f2(void *args) {
  pthread_mutex_lock (&m1);
  printf ("(%d) Soy t2: %p\n", c2++, pthread_self ());
  pthread_cond_broadcast (&cond1);
  pthread_mutex_unlock (&m1);

  return (void *)40;
}

void *f(void *args) {
  struct timespec t = {2, 0};
  pthread_mutex_lock (&m1);
  pthread_cond_wait (&cond1, &m1);
  printf ("(%d) Soy t1: %p\n", c1 ++,   pthread_self ());
  nanosleep (&t, 0);
  pthread_mutex_unlock (&m1);
  
  return (void *)32;
}

int main (int argc, char **argv) {
  void *ex = 0;
  pthread_create (&t1, 0, f, 0);
  pthread_create (&t2, 0, f2, 0);
  
  pthread_join (t1, &ex);
  printf ("t1 finishes with %d\n", (int)ex);
  pthread_join (t2, &ex);
  printf ("t2 finishes with %d\n", (int)ex);
  return 0;
}
