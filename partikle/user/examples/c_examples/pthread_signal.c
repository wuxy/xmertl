// Creating our first thread

#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

static pthread_t t1;//, t2;
static int c1 = 0;//, c2 = 0;

static void handler (int s) {
  printf ("Catching signal: %d\n",s);
}
/*
static void *f2(void *args) {
  struct timespec t = {2, 0};
  sigset_t set;

  sigemptyset(&set);
  sigaddset (&set, 11);
  pthread_sigmask (SIG_BLOCK, &set, 0);
  

  printf ("(%d) Soy t2: %p\n", c1 ++,  pthread_self ());
  pthread_kill (pthread_self(), 11);
  nanosleep (&t, 0);
  
  return (void *)0;
}
*/

static void *f(void *args) {
  //  struct timespec t = {2, 0};
  struct sigaction act;
  sigset_t set;
  int signal;
  sigemptyset(&set);
  sigaddset (&set, 11);
  pthread_sigmask (SIG_BLOCK, &set, 0);

  pthread_kill (pthread_self(), 11);

  sigemptyset(&set);
  sigaddset (&set, 11);
  sigaddset (&set, 10);
  
  if (sigwait (&set, &signal) == 0)
    printf ("despierto con: %d\n", signal);
  else
    printf ("error\n");
  
  pthread_sigmask (SIG_BLOCK, &set, 0);

  act.sa_handler = handler;
  
  sigemptyset(&act.sa_mask);
  sigaddset (&act.sa_mask, 11);
  act.sa_flags = 0;

  sigaction (11, &act, 0);
  //pthread_create (&t2, 0, f2, 0);
  printf ("(%d) Soy t1: %p\n", c1 ++,  pthread_self ());
  pthread_kill (pthread_self(), 11);
  while (1) {
    printf ("next\n");
    sigemptyset(&set);
    pthread_sigmask (SIG_SETMASK, &set, 0);
    pthread_exit (0);
  }
  //  nanosleep (&t, 0);
  //pthread_exit (0);
  return (void *)0;
}

int main (int argc, char **argv) {
  pthread_create (&t1, 0, f, 0);
  pthread_kill (t1, 10);
  pthread_join (t1, NULL);
  return 0;
}
