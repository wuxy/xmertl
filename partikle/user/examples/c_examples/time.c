// Creating our first thread

#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

int main (int argc, char **argv) {
  struct timespec tp;
  struct timeval timeval;
  //  while (1) {
  clock_gettime (CLOCK_THREAD_CPUTIME_ID, &tp);
  printf ("sec: %d nsec %d\n", tp.tv_sec, tp. tv_nsec);
  
  clock_gettime (CLOCK_MONOTONIC, &tp);    
  printf ("sec: %d nsec %d\n", tp.tv_sec, tp. tv_nsec);

  clock_gettime (CLOCK_REALTIME, &tp);    
  printf ("sec: %d nsec %d\n", tp.tv_sec, tp. tv_nsec);
    //  }
  gettimeofday (&timeval, 0);
  printf ("sec: %d usec: %d\n", timeval.tv_sec, timeval.tv_usec);
  printf ("sec: %d\n", time (0));
  printf ("sec: %d\n", time (0));
  return 0;
}
