/*
 * $FILE: xmtimer.c
 *
 * xmtimer driver
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
#include <stddef.h>
#include <irqs.h>
#include <time.h>
#include <processor.h>
#include <arch/xm_syscalls.h>

// Definitions
#define XMTIMER_EVENT 0
static timer_handler_t xmtimer_user_handler = NULL;

//-----------------//
// xmtimer_handler //
//-----------------//

static int xmtimer_irq_handler (context_t *context) {
  if (xmtimer_user_handler)
    (*xmtimer_user_handler) ();
  hw_enable_irq (XMTIMER_EVENT);
  return 0;
}
  
//----------------------//
// init_hwtimer_xmtimer //
//----------------------//

static int init_hwtimer_xmtimer (void) {
  install_irq_handler_sys (XMTIMER_EVENT, xmtimer_irq_handler);
  hw_enable_irq (XMTIMER_EVENT);
  return 0;
}

//-------------------------//
// program_hwtimer_xmtimer //
//-------------------------//

static void set_hwtimer_xmtimer (duration_t interval) {
  struct xmitimerval it;
  it.it_value = (struct xmtimespec)
    {tv_sec: interval / NSECS_PER_SEC,
     tv_nsec: interval % NSECS_PER_SEC};
  
  it.it_interval = (struct xmtimespec)
    {tv_sec: 0,
     tv_nsec: 0};
  
  xm_set_timer (&it, NULL);
}

//--------------------------//
// get_max_interval_xmtimer //
//--------------------------//

static duration_t get_max_interval_xmtimer (void) {
  // the maximum interval is of various seconds
  return (~0);
}

//--------------------------//
// get_min_interval_xmtimer //
//--------------------------//

static duration_t get_min_interval_xmtimer (void) {
  return 50000;
}

//---------------------------//
// set_timer_handler_xmtimer //
//---------------------------//

static timer_handler_t set_timer_handler_xmtimer (timer_handler_t 
						  timer_handler) {
  int flags;
  timer_handler_t old_xmtimer_user_handler = xmtimer_user_handler;
  hw_save_flags_and_cli (flags);
  xmtimer_user_handler = timer_handler;
  hw_restore_flags (flags);
  return old_xmtimer_user_handler;
}

//--------------------------//
// shutdown_hwtimer_xmtimer //
//--------------------------//

static void shutdown_hwtimer_xmtimer (void) {
  hw_disable_irq (XMTIMER_EVENT);
  install_irq_handler_sys (XMTIMER_EVENT, NULL);
}

// XM's timer
hwtimer_t xmtimer = {
  init_hwtimer: init_hwtimer_xmtimer,
  set_hwtimer: set_hwtimer_xmtimer,
  get_max_interval: get_max_interval_xmtimer,
  get_min_interval: get_min_interval_xmtimer,
  set_timer_handler: set_timer_handler_xmtimer,
  shutdown_hwtimer: shutdown_hwtimer_xmtimer,
};

//REGISTER_DRV(init_hwtimer_xmtimer, "XMTIMER");
void
xmtimerlink (void){
	init_hwtimer_xmtimer();	
}

