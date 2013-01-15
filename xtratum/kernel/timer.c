/*
 * $FILE: timer.c
 *
 * XM's timer manager
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

#include <linux_inc.h>
#include <domain.h>
#include <heap.h>
#include <hwtimer.h>
#include <sched.h>

//-------------//
// init_timers //
//-------------//

void init_timers (void) {
  sysclock.init_clock ();
  hwtimer.init_hwtimer ();
}

//---------------//
// uninit_timers //
//---------------//

void uninit_timers (void) {
  sysclock.shutdown_clock();
  hwtimer.shutdown_hwtimer();
}


//---------------------//
// timer_remove_domain //
//---------------------//

void timer_remove_domain (domain_t *d) {
  unsigned long flags;
  hw_save_flags_and_cli (&flags);
  if (d)
    HEAP_remove_element (&timer_heap, d);  
  hw_restore_flags (flags);
}

//---------------//
// timer_handler //
//---------------//

void timer_handler (void) {
  unsigned long flags;
  duration_t ctime, rtime;
  domain_t *sel_domain = NULL;

  hw_save_flags_and_cli (&flags);
  ctime = sysclock.gettime_nsec ();

  while ((sel_domain = HEAP_min (&timer_heap)) &&
         sel_domain -> time.hwtime <= ctime + hwtimer.get_min_interval ()) {
    sel_domain = HEAP_remove_min (&timer_heap);

    set_bit (sel_domain -> events -> pending_events, 
	     hwtimer.timer_event);

    if (sel_domain -> time.period) {
      sel_domain -> time.hwtime =
        sel_domain -> time.period + ctime;
      HEAP_insert (&timer_heap, sel_domain);
    }
  }

  if (sel_domain) {
    rtime = sel_domain -> time.hwtime - ctime;
    if (rtime < hwtimer.get_max_interval())
      hwtimer.set_hwtimer (rtime);
    else 
      hwtimer.set_hwtimer (hwtimer.get_max_interval ());
  } 
  hw_restore_flags (flags);
}

//------------//
// sethwtimer //
//------------//

void sethwtimer (duration_t dtime, duration_t dperiod) {
  unsigned long flags;
  
  duration_t ctime;

  hw_save_flags_and_cli (&flags);
 
  ctime = sysclock.gettime_nsec ();
  xm_current_domain -> time.hwtime = dtime + ctime;

  if (HEAP_insert (&timer_heap, xm_current_domain) != 0) {
    printk ("xm_sethwtimer error: no more virtual timers can be created.");
    hw_restore_flags (flags);
    return;
  }
  
  xm_current_domain -> time.period = dperiod;

  hw_restore_flags (flags);

  timer_handler ();

  // this timer may be out of time
  xm_sched ();
}

