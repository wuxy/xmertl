/*
 * $FILE: timer.h
 *
 * Timer definition
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

#ifndef _XM_TIMER_H_
#define _XM_TIMER_H_

#ifndef UXMLIB
#include <hwtimer.h>
#include <domain.h>

extern void init_timers (void);
extern void uninit_timers (void);
extern void timer_handler (void);
extern void sethwtimer (duration_t, duration_t);
extern void timer_remove_domain (struct domain_struct *d);

// The following structure is used by each domain to know the
// state of each enabled timer
typedef struct time_inf_struct {
  hwtime_t hwtime;
  hwtime_t period;
} time_inf_t;

#endif

struct xmtimespec {
  int tv_sec; /* Seconds.  */
  long int tv_nsec; /* Nanoseconds.  */
};

struct xmitimerval {
  struct xmtimespec it_interval; /* next value */
  struct xmtimespec it_value; /* current value */
};

#endif
