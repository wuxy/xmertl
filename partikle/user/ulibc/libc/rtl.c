/*
 * $FILE: rtl.c
 *
 * RTLinux _np (Non Portable) Posix
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
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

#include <config.h>
#include <errno.h>
#include <sched.h>
#include <time.h>
#include <sys/types.h>
#include <stdio.h>
#include <rtl.h>
#include <syscalls.h>

#define NSECS_PER_SEC 1000000000ULL	// fix me

//-------------------//
// pthread_delete_np //
//-------------------//

_syscall1 (int, pthread_delete_np, pthread_t, thread);

//----------------------//
// pthread_setperiod_np //
//----------------------//

_syscall2(int, pthread_setperiod_np, pthread_t, thread, 
	  struct timespec *, value);

//----------------------//
// pthread_getperiod_np //
//----------------------//

_syscall2 (int, pthread_getperiod_np, pthread_t, thread, duration_t *, period);

//-----------------//
// clock_gethrtime //
//-----------------//

hwtime_t clock_gethrtime (clockid_t clock_id) {
  struct timespec res;
  if (clock_gettime (clock_id, &res) < 0)
    return 0;
  
  return res.tv_sec * 60000000 + res.tv_nsec;
}

//--------------------------//
// pthread_make_periodic_np //
//--------------------------//

int pthread_make_periodic_np (pthread_t p, hwtime_t start_time, 
			      hwtime_t period) {
  struct timespec value = {
    tv_sec: period / NSECS_PER_SEC,
    tv_nsec: period % NSECS_PER_SEC
  };
	
  return pthread_setperiod_np (p, &value);
}

//-----------------//
// pthread_wait_np //
//-----------------//

int pthread_wait_np (void) {
  pthread_t self = pthread_self();
  duration_t period;
  
  pthread_getperiod_np (self, &period);
  struct timespec req = {
    tv_sec: period / NSECS_PER_SEC,
    tv_nsec: period % NSECS_PER_SEC
  };
  
  return nanosleep (&req, 0);
}

//-------------------//
// pthread_wakeup_np //
//-------------------//

// see pthread_signal example
int pthread_wakeup_np(pthread_t thread) {
  return pthread_kill(thread, WAKEUP_SIGNAL);
}

//--------------------//
// pthread_suspend_np //
//--------------------//

int pthread_suspend_np(pthread_t thread) {
  return pthread_kill(thread, SLEEP_SIGNAL);
}
