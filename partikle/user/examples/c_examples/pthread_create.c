// Creating our first thread

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include <hwirqs.h>


pthread_t t1, t2;

int c1 = 0, c2 = 0;
void *f2(void *args) {
  struct timespec t = {2, 0}, tp;
  
  while (1) {
    clock_gettime (CLOCK_THREAD_CPUTIME_ID, &tp);
    printf ("sec: %d nsec %d\n", tp.tv_sec, tp. tv_nsec);
    printf ("(%d) Soy t2: %p\n", c2++, pthread_self ());
    nanosleep (&t, 0);
    clock_gettime (CLOCK_THREAD_CPUTIME_ID, &tp);
    printf ("sec: %d nsec %d\n", tp.tv_sec, tp. tv_nsec);
  }
  return (void *)0;
}

void h1(void *a) {
  printf ("Soy h1: %x\n", a);
}

void h2(void *a) {
  printf ("Soy h2: %x\n", a);
}
static void handler (int s) {
  printf ("Catching signal: %d\n",s);
}

void *f(void *args) {
  struct timespec t = {1, 0}, r;
  // pthread_create (&t2, 0, f2, 0);
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, 0);
  pthread_cleanup_push (h1, (void *)12);
  pthread_cleanup_push (h2, (void *)4);
  // pthread_cleanup_pop (1);
  //pthread_cleanup_pop (1);
  //pthread_exit (1);
  while (1) {
    printf ("(%d) Soy t1: %p\n", c1 ++,   pthread_self ());
    if (nanosleep (&t, &r) == -1) 
      printf ("rem: seg %d nseg: %d\n", r.tv_sec, r.tv_nsec);
    pthread_yield ();
    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, 0);
  }
  pthread_join (t2, 0);
  return (void *)0;
}

int main (int argc, char **argv) {
  struct timespec t = {1, 0};
  pthread_create (&t1, 0, f, 0);
  nanosleep (&t, 0);
  pthread_kill (t1, 10);
  printf ("Hello World\n");
  //  pthread_cancel (t1);
  pthread_join (t1, 0);
  return 0;
}
