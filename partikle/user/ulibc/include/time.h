/*
 * $FILE: time.h
 *
 * Time types
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
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

#ifndef _ULIBC_TIME_H_
#define _ULIBC_TIME_H_

#include <kernel_inc/time.h>
#include <sys/types.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

extern time_t time (time_t *t);
extern int nanosleep (const struct timespec *req, struct timespec *rem);
extern int usleep (useconds_t usec);
extern time_t mktime (struct tm *const t);
extern time_t timegm (struct tm *const t);
extern struct tm *localtime_r (const time_t *timep, struct tm *result);
extern struct tm *localtime (const time_t *timep);
extern struct tm *gmtime (const time_t *timep);
extern int clock_settime (clockid_t clock_id, const struct timespec *tp);
extern int clock_gettime (clockid_t clock_id, struct timespec *tp);
extern int clock_getres (clockid_t clock_id, struct timespec *res);
#include <signal.h>
extern int timer_create (clockid_t clockid, struct sigevent *evp,
			 timer_t *timerid);
extern int timer_delete (timer_t timerid);
extern int timer_settime (timer_t timerid, int flags,
			  const struct itimerspec *value,
			  struct itimerspec *ovalue);
extern int timer_gettime (timer_t timerid,
			  struct itimerspec *value);
extern int timer_getoverrun (timer_t timerid);

END_DECLS

#endif
