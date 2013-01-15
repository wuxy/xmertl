/*
 * $FILE: hwtimers.h
 *
 * Low level timers interface
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

#ifndef _HWTIMERS_H_
#define _HWTIMERS_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <arch/arch_types.h>

#define NSECS_PER_SEC 1000000000ULL
#define USECS_PER_SEC 1000000UL

typedef int (*timer_handler_t) (void);

typedef struct {
  int (*init_clock) (void);
  hwtime_t (*gettime_hwt) (void);
  duration_t (*gettime_nsec) (void);
  hwtime_t (*hwtime2duration) (hwtime_t);
  duration_t (*getres) (void);
  void (*shutdown_clock) (void);
} system_clock_t;

/* The system only should have one monotonically increasing clock */
extern system_clock_t *monotonic_clock;
extern system_clock_t *realtime_clock;

// The following structure defines each hwtimer existing in the system
typedef struct hwtimer_struct {
  int (*init_hwtimer) (void);
  void (*set_hwtimer) (duration_t);
  // This is the maximum value to be programmed
  duration_t (*get_max_interval) (void);
  duration_t (*get_min_interval) (void);
  timer_handler_t (*set_timer_handler) (timer_handler_t timer_handler);
  void (*shutdown_hwtimer) (void);
} hwtimer_t;


// Array of available hwtimers
extern hwtimer_t *hwtimers[];

// returns the number of available hwtimers in the system
extern int getnrhwtimers (void);
extern hwtimer_t *get_best_hwtimer (void);

#endif
