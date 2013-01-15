/*
 * $FILE: time.c
 *
 * Clock-related functions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *
 * $LICENSE:  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

/*
  Added tracing support to the sleep_for function - Salvador Peiro
*/

#include <errno.h>
#include <time.h>
#include <sched.h>
#include <hwtimers.h>
#include <heap.h>
#include <pthread.h>
#include <processor.h>
#include <stdio.h>
#include <trace.h>
#include <signal.h>
#include <string.h>

#define NEED_REESCHED 1

static hwtimer_t *hwtimer = NULL;

// PaRTiKle's timer heap
static heap_t timer_heap;

// The timer array
ktimer_t timer_array [MAX_TIMERS];

/////////////////////////////////////////////

//----------------------//
// read_monotonic_timer //
//----------------------//

static duration_t read_monotonic_timer (pthread_t t) {
  return monotonic_clock->gettime_nsec ();
}

//---------------------//
// read_realtime_timer //
//---------------------//

static duration_t read_realtime_timer (pthread_t t) {
  return realtime_clock->gettime_nsec ();
}

//---------------------------//
// read_thread_cputime_timer //
//---------------------------//

static duration_t read_thread_cputime_timer (pthread_t t) {
  return t->used_time+monotonic_clock->gettime_nsec()-t->starting_time;
}

//---------------//
// action_wakeup //
//---------------//

int action_wakeup (ktimer_t *ktimer) {
  activate_thread (ktimer -> owner);

  return NEED_REESCHED; // resched
}

//-------------//
// action_kill //
//-------------//

static int action_kill (ktimer_t *ktimer) { 
  pthread_kill_sys (ktimer->owner, ktimer->sigevent.sigev_signo);
  return NEED_REESCHED; // resched
}

///////////////////////////////////////////////////

//---------------//
// clock_handler //
//---------------//

static int update_hwtimer (void);
static int clock_handler (void) {
  if (update_hwtimer () == NEED_REESCHED)
    scheduling ();

  return 0;
}

//-----------//
// init_time //
//-----------//

int init_time (void) {
  // Asking for the best hwtimer available in the system
  if (!(hwtimer = get_best_hwtimer ())) return -1;

  hwtimer -> set_timer_handler (clock_handler);

  memset ((unsigned char *) &timer_array [0], 0, 
	  sizeof (ktimer_t) * MAX_TIMERS);
  
  HEAP_init (&timer_heap);
  return 0;
}

//--------------//
// alloc_ktimer //
//--------------//

int alloc_ktimer (clockid_t clockid, pthread_t thread, 
		  int (*act) (ktimer_t *)) {
  unsigned long flags;
  timer_t timerid;
  ktimer_t *ktimer;

  hw_save_flags_and_cli (flags);
  for (timerid = 0; timerid < MAX_TIMERS; timerid ++)
    if (!(timer_array [timerid].flags & TIMER_IN_USE)) break;
  
  if (timerid >= MAX_TIMERS) {
    hw_restore_flags (flags);
    return -1;
  }
  
  ktimer = &timer_array [timerid];
  
  memset ((unsigned char *)ktimer, 0, sizeof (ktimer_t));
  ktimer->owner = thread;
  ktimer->flags = TIMER_IN_USE;
    
  switch (clockid) {
  case CLOCK_REALTIME:
    ktimer->read_clock = read_realtime_timer;
    break;
  case CLOCK_THREAD_CPUTIME_ID:
    ktimer->read_clock = read_thread_cputime_timer;
    break;
  case CLOCK_MONOTONIC:
  default:
    ktimer->read_clock = read_monotonic_timer;
    break;
  }
  ktimer->action = act;
  
  hw_restore_flags (flags);
  
  return timerid;
}

//-------------//
// free_ktimer //
//-------------//

void free_ktimer (ktimer_t *ktimer) {
  unsigned long flags;
  
  hw_save_flags_and_cli (flags);
  HEAP_remove_element (&timer_heap, ktimer);
  ktimer->flags = 0;
  hw_restore_flags (flags);
}

//-----------//
// arm_timer //
//-----------//

int arm_timer (ktimer_t *ktimer, duration_t value, duration_t interval) {
  int flags;
  
  if (!ktimer) return -1;
  if (!(ktimer->flags & TIMER_IN_USE)) return -1;

  hw_save_flags_and_cli (flags);
  ktimer->timer_value = value;
  
  if ((ktimer->flags & TIMER_ABSTIME) != TIMER_ABSTIME) {
    ktimer->timer_value +=
      ktimer->read_clock (ktimer->owner);
  }
  
  ktimer -> flags |= TIMER_ARMED;
  ktimer -> timer_interval = interval;
  
  HEAP_insert (&timer_heap, ktimer);
  
  hw_restore_flags (flags);

  //  update_hwtimer ();

  return 0;
}

//--------------//
// disarm_timer //
//--------------//

void disarm_timer (timer_t timerid) {
  unsigned long flags;
    
  hw_save_flags_and_cli (flags);
  HEAP_remove_element (&timer_heap, &timer_array [timerid]);
  timer_array [timerid].flags &= ~TIMER_ARMED;
  hw_restore_flags (flags);
}

// This function traverses the heap looking for, on the one hand,
// the already triggered clocks; and the next activation event on the
// other one.

//----------------//
// update_hwtimer //
//----------------//

static int update_hwtimer (void) {
  int flags, action = 0;
  ktimer_t *ktimer;
  duration_t ctime=0, next_time;

  hw_save_flags_and_cli (flags);

  ktimer = HEAP_min (&timer_heap);
//   HEAP_print (&timer_heap);
  if (ktimer)
    ctime = ktimer->read_clock (ktimer->owner);
  
  while (ktimer && ktimer -> timer_value <= ctime) {
    action = ktimer -> action (ktimer);
    HEAP_remove_min (&timer_heap);
    
    // Reprogramming the timer
    if (ktimer -> timer_interval > 0 &&
	GET_THREAD_STATE(ktimer->owner) != FINISHED_THREAD) {
      ktimer -> timer_value = ktimer -> timer_interval + ctime;
      HEAP_insert (&timer_heap, ktimer);
    } else {
      // Disarming the timer
      ktimer->flags &= ~TIMER_ARMED;
    }
    
    ktimer = HEAP_min (&timer_heap);
    
    if (ktimer)
      ctime = ktimer->read_clock (ktimer->owner);    
  }

  if (!ktimer) {
    hw_restore_flags (flags);
    return action;
  }

  next_time = ktimer -> timer_value - ctime;
//   printf ("NExt time = %lld\n", next_time);

  if ((unsigned long long) next_time < hwtimer ->  get_min_interval ())
    next_time = hwtimer ->  get_min_interval ();
  else if ((unsigned long long) next_time > hwtimer -> get_max_interval ())
    next_time = hwtimer -> get_max_interval ();

  hwtimer -> set_hwtimer (next_time);

  hw_restore_flags (flags);

  return action;
}

//-----------//
// sleep_for //
//-----------//

// This function sleeps a thread for a relative amount of time

duration_t sleep_for (duration_t d) {
  duration_t ctime;
  int flags;
  ktimer_t *ktimer = current_thread -> sleep_ktimer;

  hw_save_flags_and_cli (flags);
  
#ifdef CONFIG_PORT_DEVTRACE
  log_event ((struct Traceevent) {
	etype: readye,
	id: current_thread->trace_id,
	time: monotonic_clock->gettime_nsec (),
  });
#endif

  arm_timer (ktimer, d, 0);
  suspend_thread (current_thread);

#ifdef CONFIG_PORT_DEVTRACE
  log_event ((struct Traceevent) {
	etype: readyb,
	id: current_thread->trace_id,
	time: monotonic_clock->gettime_nsec (),
  });
#endif

  hw_restore_flags (flags);
  update_hwtimer ();
  scheduling ();

  //hw_save_flags_and_cli (flags);
  //disarm_timer ();
  //HEAP_remove_element (&timer_heap, ktimer);
  //hw_restore_flags (flags);

  ctime = ktimer -> read_clock(ktimer->owner);

  if (d > ctime)
    return (d - ctime);
  return 0;
}

//---------------//
// nanosleep_sys //
//---------------//

asmlinkage int nanosleep_sys (const struct timespec *req, 
			      struct timespec *rem) {
  duration_t treq = req -> tv_sec * NSECS_PER_SEC + req -> tv_nsec, 
    d = sleep_for (treq);

  if (d) {
    if (rem)
      *rem = (struct timespec){tv_sec: d / NSECS_PER_SEC,
			       tv_nsec: d % NSECS_PER_SEC};
    SET_ERRNO (EINTR);
    return -1;
  }

  return 0;
}

//------------//
// usleep_sys //
//------------//

asmlinkage int usleep_sys (useconds_t usec) {
  duration_t d = sleep_for (usec * 1000);

  if (d) {
    SET_ERRNO (EINTR);
    return -1;
  }
  return 0;
}

//-------------------//
// clock_settime_sys //
//-------------------//

asmlinkage int clock_settime_sys (clockid_t clock_id, 
				  const struct timespec *tp) {
  SET_ERRNO (EPERM);
  return -1;
}

//-------------------//
// clock_gettime_sys //
//-------------------//

asmlinkage int clock_gettime_sys (clockid_t clock_id, struct timespec *tp) {
  duration_t d = 0;

  if (!tp) {
    SET_ERRNO (EFAULT);
    return -1;
  }

  switch (clock_id) {
  case CLOCK_REALTIME:
    d = realtime_clock->gettime_nsec ();
    break;
  case CLOCK_MONOTONIC:
    d = monotonic_clock->gettime_nsec ();
    break;
  case CLOCK_THREAD_CPUTIME_ID:
    d = current_thread -> used_time + monotonic_clock->gettime_nsec() 
      - current_thread -> starting_time;
    break;
  default:
    SET_ERRNO (EINVAL);
    return -1;
  }

  *tp = (struct timespec) {tv_sec: d / NSECS_PER_SEC,
			   tv_nsec: d % NSECS_PER_SEC};

  return 0;
}

//------------------//
// clock_getres_sys //
//------------------//

asmlinkage int clock_getres_sys (clockid_t clock_id, struct timespec *res) {
  duration_t d = 0;
  switch (clock_id) {
  case CLOCK_REALTIME:
    d = realtime_clock->getres ();
    break;
  case CLOCK_MONOTONIC:
  case CLOCK_THREAD_CPUTIME_ID:
    d = monotonic_clock->getres ();
    break;
  default:
    SET_ERRNO (EINVAL);
    return -1;
  }

  if (res) {
    *res = (struct timespec) {tv_sec: d / NSECS_PER_SEC,
			     tv_nsec: d % NSECS_PER_SEC};
  }

  return 0;
}

//----------//
// time_sys //
//----------//

asmlinkage time_t time_sys (time_t *t) {
  duration_t d = realtime_clock->gettime_nsec () / NSECS_PER_SEC;
  
  if (t)
    *t = (time_t) d;
  
  return (time_t) d;
}

//------------------//
// gettimeofday_sys //
//------------------//

asmlinkage int gettimeofday_sys (struct timeval *tv, struct timezone *tz) {
  duration_t d = realtime_clock->gettime_nsec () / 1000;
  if (!tv) {
    SET_ERRNO (EFAULT);
    return -1;
  }
  
  *tv = (struct timeval) {
    tv_sec: d / USECS_PER_SEC,
    tv_usec: d % USECS_PER_SEC,
  };

  return 0;
}

//------------------//
// timer_create_sys //
//------------------//

asmlinkage int timer_create_sys (clockid_t clockid, struct sigevent *evp,
				 timer_t *timerid) {

  if (clockid != CLOCK_REALTIME && clockid != CLOCK_MONOTONIC &&
      clockid != CLOCK_THREAD_CPUTIME_ID) {
    SET_ERRNO (EINVAL);
    return -1;
  }

  *timerid = alloc_ktimer (clockid, current_thread, action_kill);
  if ((*timerid) >= MAX_TIMERS) {
    SET_ERRNO (EAGAIN);
    return -1;
  }

  if (evp) {
    timer_array [(*timerid)].sigevent = *evp;
  } else {
    timer_array [(*timerid)].sigevent = (struct sigevent) {
      sigev_notify: 0,
      sigev_signo: SIGALARM,
      sigev_value: (union sigval) {sival_int: (*timerid)}
    };
  }
  
  return 0;
}

//------------------//
// timer_delete_sys //
//------------------//

asmlinkage int timer_delete_sys (timer_t timerid) {
  
  if (timerid >= MAX_TIMERS || 
      !(timer_array[timerid].flags & TIMER_IN_USE)) {
    SET_ERRNO (EAGAIN);
    return -1;
  }

  // the ktimer is disarmed by the free_ktimer function so
  // it is not necessary to call disarm_ktimer
  free_ktimer (&timer_array [timerid]);
  
  return 0;
}

//-------------------//
// timer_settime_sys //
//-------------------//

asmlinkage int timer_settime_sys (timer_t timerid, int flags, 
				  const struct itimerspec *value, 
				  struct itimerspec *ovalue) {
  if (timerid >= MAX_TIMERS || 
      !(timer_array[timerid].flags & TIMER_IN_USE)) {
    SET_ERRNO (EAGAIN);
    return -1;
  }

  if (timer_array [timerid].flags & TIMER_ARMED) {
    if (ovalue)
      *ovalue = (struct itimerspec) {
	it_interval: (struct timespec)
	{tv_sec: timer_array[timerid].timer_interval/NSECS_PER_SEC,
	 tv_nsec: timer_array[timerid].timer_interval%NSECS_PER_SEC},
	it_value: (struct timespec)
	{tv_sec: timer_array[timerid].timer_value/NSECS_PER_SEC,
	 tv_nsec: timer_array[timerid].timer_value%NSECS_PER_SEC}
      };

    disarm_timer (timerid);
  }
  
  // Setting the flags
  timer_array [timerid].flags &= ~TIMER_ABSTIME;
  timer_array [timerid].flags |= (flags & TIMER_ABSTIME);

  if (value && (value -> it_value.tv_sec | value -> it_value.tv_nsec)) {
    duration_t v = value->it_value.tv_sec*NSECS_PER_SEC +
      value->it_value.tv_nsec;
    duration_t i =  value->it_interval.tv_sec*NSECS_PER_SEC +
      value->it_interval.tv_nsec;

    arm_timer (&timer_array [timerid], v, i);
    update_hwtimer ();
  }

  return 0;
}

//-------------------//
// timer_gettime_sys //
//-------------------//

asmlinkage int timer_gettime_sys (timer_t timerid, struct itimerspec *value){
  if (timerid >= MAX_TIMERS || 
      !(timer_array[timerid].flags & TIMER_IN_USE)) {
    SET_ERRNO (EAGAIN);
    return -1;
  }
  
  if (value) {
    if ((timer_array [timerid].flags & TIMER_ARMED)== TIMER_ARMED) {
      duration_t ctime = timer_array[timerid].
	read_clock (timer_array[timerid].owner);
      *value = (struct itimerspec) {
	it_interval: (struct timespec)
	{tv_sec: timer_array[timerid].timer_interval/NSECS_PER_SEC,
	 tv_nsec: timer_array[timerid].timer_interval%NSECS_PER_SEC},
	it_value: (struct timespec)
	{tv_sec: (timer_array[timerid].timer_value - ctime)/NSECS_PER_SEC,
	 tv_nsec: (timer_array[timerid].timer_value - ctime)%NSECS_PER_SEC}
      };
    } else
      memset ((unsigned char *)value, 0, sizeof (struct itimerspec));
  }
  

  return 0;
}

//----------------------//
// timer_getoverrun_sys //
//----------------------//

// Not implemented yet
asmlinkage int timer_getoverrun_sys (timer_t timerid) {
 
  if (timerid >= MAX_TIMERS || 
      !(timer_array[timerid].flags & TIMER_IN_USE)) {
    SET_ERRNO (EAGAIN);
    return -1;
  }
  return 0;
}
