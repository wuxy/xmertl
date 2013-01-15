/*
 * $FILE: hwtimer.h
 *
 * Hwtimer definition
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

#ifndef _XM_HWTIMER_H_
#define _XM_HWTIMER_H_

#include <arch/hwtimer.h>

#define duration_t long long
#define hwtime_t long long

#define NSECS_PER_SEC 1000000000LL
#define USECS_PER_SEC 1000000L

// System's clock
typedef struct sysclock_struct {
  int (*init_clock) (void);
  hwtime_t (*gettime_hwt) (void);
  duration_t (*gettime_nsec) (void);
  duration_t (*hwtime2duration) (hwtime_t);
  hwtime_t (*duration2hwtime) (duration_t);
  duration_t (*getres) (void);
  void (*shutdown_clock) (void);
} sysclock_t;

// The following structure defines each hwtimer existing in the system
typedef struct hwtimer_struct {
  int (*init_hwtimer) (void);
  void (*set_hwtimer) (duration_t);
  // This is the maximum value to be programmed
  duration_t (*get_max_interval) (void);
  duration_t (*get_min_interval) (void);
  void (*shutdown_hwtimer) (void);
  int timer_event;
} hwtimer_t;

extern sysclock_t sysclock;
extern hwtimer_t hwtimer;

#endif
