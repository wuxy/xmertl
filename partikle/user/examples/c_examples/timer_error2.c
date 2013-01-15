/*
 * $FILE: timer_error.c
 *
 * Reproducció d'error en timers
 *
 * $VERSION: 1.0
 *
 * Author: Vicent Brocal  <vibrotor@doctor.upv.es>
 *
 * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#define th_launch(th, attr, func)  {if (pthread_create((th), (attr), func, NULL)) {\
                                      perror(#func);\
                                      exit (-1);\
}};


//----------//
// th_sleep //
//----------//

void * th_sleep(void *args)
{
  struct timespec t = {10, 0};

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  /* Do nothing */
  while (1){
    nanosleep(&t, NULL);
  }

  return NULL;
}


//------------------//
// error_predecesor //
//------------------//

int error_predecesor (void)
{
  pthread_t th;

  th_launch (&th, NULL, th_sleep);
//   pthread_cancel (th);
//   pthread_join (th, NULL);

  return 0;
}

#define IT_SEC 2
int timer_error ()
{
  timer_t timer;
  struct itimerspec it = {{0,0}, {IT_SEC,0}};
  sigset_t wset, oset;
  int sig;

  printf ("START\n");
  timer_create (CLOCK_REALTIME, NULL, &timer);
  assert (!timer_settime (timer, 0, &it, NULL));

  sigemptyset (&wset);
  sigaddset (&wset, SIGALARM);

  assert (!pthread_sigmask (SIG_BLOCK, &wset, &oset));
  printf ("Wait for timer expiration: %d sec:\t\t", IT_SEC);
  assert (!sigwait (&wset, &sig));
  assert (sig == SIGALARM);
  printf ("OK\n");

  assert (!pthread_sigmask (SIG_SETMASK, &oset, NULL));

  return 0;
}


int main (void)
{
  error_predecesor ();
  timer_error ();

  return 0; 
}
