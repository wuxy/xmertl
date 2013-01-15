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

#include <hwtimers.h>

#define i386_HWTIMERS 1

extern hwtimer_t pit_tsc;
extern system_clock_t tsc_clock;

hwtimer_t *hwtimers[i386_HWTIMERS] = {
  &pit_tsc
};

system_clock_t *monotonic_clock = &tsc_clock;
system_clock_t *realtime_clock = &tsc_clock;

//---------------//
// getnrhwtimers // 
//---------------//

int getnrhwtimers (void) {
  return i386_HWTIMERS;
}

//------------------//
// get_best_hwtimer //
//------------------//

hwtimer_t *get_best_hwtimer (void) {
  return &pit_tsc;
}
