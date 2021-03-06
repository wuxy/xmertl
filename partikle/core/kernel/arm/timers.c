/*
 * $FILE: timers.c
 *
 * Timers
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

#include <config.h>
#include <hwtimers.h>

#ifdef CONFIG_BOARD_INTEGRATORCP
#define ICP_HWTIMERS 1

extern hwtimer_t icp_timer;
extern system_clock_t icp_timer_clock;

hwtimer_t *hwtimers[ICP_HWTIMERS] = {
  &icp_timer
};

system_clock_t *monotonic_clock = &icp_timer_clock;
system_clock_t *realtime_clock = &icp_timer_clock;

#endif

//---------------//
// getnrhwtimers //
//---------------//

int getnrhwtimers (void) {
#ifdef CONFIG_BOARD_INTEGRATORCP
  return ICP_HWTIMERS;
#else
  return 0;
#endif
}

//------------------//
// get_best_hwtimer //
//------------------//

hwtimer_t *get_best_hwtimer (void) {
#ifdef CONFIG_BOARD_INTEGRATORCP
  return &icp_timer;
#else
  return 0;
#endif
}
