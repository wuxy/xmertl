#include <stdio.h>
#include <pthread.h>

pthread_mutex_t m1;

int main (void) {
  pthread_mutexattr_t at;
  pthread_mutexattr_init (&at);
  pthread_mutexattr_settype (&at, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutexattr_setprotocol (&at, PTHREAD_PRIO_PROTECT);
  pthread_mutex_init (&m1, &at);
  
  pthread_mutex_lock (&m1);
  printf ("Locked 1\n");
  //  pthread_mutex_lock (&m1);
  printf ("Locked 2\n");
  pthread_mutex_setprioceiling (&m1, 15, 0);
  printf ("Ceiling set to 15\n");
  //  pthread_mutex_unlock (&m1);
  printf ("Unlocked 2\n");
  pthread_mutex_unlock (&m1);
  printf ("Unlocked 1\n");

  return 0;
}
