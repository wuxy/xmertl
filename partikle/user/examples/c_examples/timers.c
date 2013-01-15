#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>


static void handler (int s) {
  printf ("Catching signal: %d\n",s);
}

void *thread_routine (void *args) {
  while (1) usleep (1);
  return 0;
}

int main (int argc, char **argv) {
	timer_t deadline;	
	struct timespec tp = {3,0};
	struct itimerspec it = {{5,0}, {2,0}};
	
	struct sigaction act;
	sigset_t set;
  

	sigemptyset(&set);
	pthread_sigmask (SIG_SETMASK, &set, 0);
	act.sa_handler = handler;
	sigemptyset (&act.sa_mask);
	sigaddset (&act.sa_mask, SIGALARM);
	act.sa_flags = 0;
	sigaction (SIGALARM, &act, 0);
	
	timer_create (CLOCK_MONOTONIC, 0, &deadline);
	timer_settime(deadline, 0, &it,  0);
	nanosleep (&tp, 0);
	printf ("Hello\n");
	
	while (1);
	return 0;
}
