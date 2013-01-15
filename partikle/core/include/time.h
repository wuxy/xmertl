/*
 * $FILE: time.h
 *
 * time types
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

#ifndef _TIME_H_
#define _TIME_H_

#ifdef _KERNEL_
#include <arch/arch_types.h>
#else
#include <sys/types.h>
#endif

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_THREAD_CPUTIME_ID 2

#define TIMER_ABSTIME 0x1

struct timespec {
  time_t tv_sec;              /* Seconds.  */
  long int tv_nsec;           /* Nanoseconds.  */
};

struct itimerval {
  struct timespec it_interval; /* next value */
  struct timespec it_value;    /* current value */
};

struct tm {
  int     tm_sec;         /* seconds */
  int     tm_min;         /* minutes */
  int     tm_hour;        /* hours */
  int     tm_mday;        /* day of the month */
  int     tm_mon;         /* month */
  int     tm_year;        /* year */
  int     tm_wday;        /* day of the week */
  int     tm_yday;        /* day in the year */
  int     tm_isdst;       /* daylight saving time */
};

struct timezone {
  int tz_minuteswest;   /* minutes west of Greenwich */
  int tz_dsttime;       /* type of dst correction */
};

struct timeval {
  time_t tv_sec;
  suseconds_t tv_usec;
};

struct itimerspec {
  struct timespec it_interval;
  struct timespec it_value;
};

#ifdef _KERNEL_
#include <linkage.h>
#include <types.h>
#include <signal.h>

#define TIMER_IN_USE  0x100
#define TIMER_ARMED   0x200

typedef struct ktimer_struct {
  pthread_t owner;
  hwtime_t timer_value;  
  hwtime_t timer_interval; // Only necessary when programming the timer in
		     // periodic mode
  clockid_t clockid;

  /* State of the current timer */
  int flags;
  
  /* Actions performed by the timer, e.g. read the clock */
  duration_t (*read_clock) (pthread_t);
  int (*action) (struct ktimer_struct *);

  //struct itimerspec value;
  struct sigevent sigevent;
} ktimer_t;

extern ktimer_t timer_array [MAX_TIMERS];

extern time_t mktime (struct tm *const t);
extern time_t time_to_seconds_since_epoch (void);
extern int init_time (void);
extern duration_t sleep_for (duration_t d);
extern asmlinkage int nanosleep_sys (const struct timespec *req, 
				     struct timespec *rem);
extern asmlinkage int usleep_sys (useconds_t usec);
extern asmlinkage int clock_settime_sys (clockid_t clock_id, 
					 const struct timespec *tp);
extern asmlinkage int clock_gettime_sys (clockid_t clock_id, 
					 struct timespec *tp);
extern asmlinkage int clock_getres_sys (clockid_t clock_id, 
					struct timespec *res);
extern asmlinkage time_t time_sys (time_t *t);
extern asmlinkage int gettimeofday_sys (struct timeval *tv, 
					struct timezone *tz);
extern asmlinkage int timer_create_sys (clockid_t clockid, 
					struct sigevent *evp,
					timer_t *timerid);
extern asmlinkage int timer_delete_sys (timer_t timerid);
extern asmlinkage int timer_settime_sys (timer_t timerid, int flags, 
					 const struct itimerspec *value, 
					 struct itimerspec *ovalue);
extern asmlinkage int timer_gettime_sys (timer_t timerid, 
					 struct itimerspec *value);
extern asmlinkage int timer_getoverrun_sys (timer_t timerid);

extern int action_wakeup (ktimer_t *ktimer);
// extern int action_kill (ktimer_t *ktimer);
extern int alloc_ktimer (clockid_t clockid, pthread_t thread, 
	  	         int (*act) (ktimer_t *));
extern void free_ktimer (ktimer_t *ktimer);
extern int arm_timer (ktimer_t *ktimer, duration_t value, 
		      duration_t interval);
extern void disarm_timer (timer_t timerid);

#endif
#endif
