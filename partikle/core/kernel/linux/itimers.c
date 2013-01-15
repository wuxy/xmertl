/*
 * $FILE: itimers.c
 *
 * itimers driver
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
#include <arch/linux_syscalls.h>
#include <dev.h>

// Definitions

static timer_handler_t itimers_user_handler = NULL;

#define ITIMERS_LINUX_SIGNAL 14

//-----------------//
// itimers_handler //
//-----------------//

static int itimers_irq_handler (context_t *context) {
  if (itimers_user_handler)
    (*itimers_user_handler) ();

  hw_enable_irq (ITIMERS_LINUX_SIGNAL);
  
  return 0;
}

//----------------------//
// init_hwtimer_itimers //
//----------------------//

static int init_hwtimer_itimers (void) {
  install_irq_handler_sys (ITIMERS_LINUX_SIGNAL, itimers_irq_handler);
  hw_enable_irq (ITIMERS_LINUX_SIGNAL);

  return 0;
}

//-------------------------//
// program_hwtimer_itimers //
//-------------------------//

// BUG: Linux considers the nsecs as usecs

static void set_hwtimer_itimers (duration_t interval) {
  struct linux_itimerval it;

  if (interval) {
    it.it_value.tv_sec = interval / NSECS_PER_SEC;
    it.it_value.tv_usec = (interval % NSECS_PER_SEC) / 1000;
  
    if (!it.it_value.tv_sec && !it.it_value.tv_usec)
      it.it_value.tv_usec = 1;
  } else {
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 0;
  }
  
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 0;

  linux_setitimer (0, &it, NULL);
}

//--------------------------//
// get_max_interval_itimers //
//--------------------------//

static duration_t get_max_interval_itimers (void) {
  // the maximum interval is of various seconds
  return (~0);
}

//--------------------------//
// get_min_interval_itimers //
//--------------------------//

static duration_t get_min_interval_itimers (void) {
  return 50000;
}

//---------------------------//
// set_timer_handler_itimers //
//---------------------------//

static timer_handler_t set_timer_handler_itimers (timer_handler_t 
						  timer_handler) {
  int flags;
  timer_handler_t old_itimers_user_handler = itimers_user_handler;
  hw_save_flags_and_cli (flags);
  itimers_user_handler = timer_handler;
  hw_restore_flags (flags);
  return old_itimers_user_handler;
}

//--------------------------//
// shutdown_hwtimer_itimers //
//--------------------------//

static void shutdown_hwtimer_itimers (void) {
  hw_disable_irq (ITIMERS_LINUX_SIGNAL);
  install_irq_handler_sys (ITIMERS_LINUX_SIGNAL, NULL);
}

// Linux's itimers
hwtimer_t itimers = {
  init_hwtimer: init_hwtimer_itimers,
  set_hwtimer: set_hwtimer_itimers,
  get_max_interval: get_max_interval_itimers,
  get_min_interval: get_min_interval_itimers,
  set_timer_handler: set_timer_handler_itimers,
  shutdown_hwtimer: shutdown_hwtimer_itimers,
};

//REGISTER_DRV(init_hwtimer_itimers, 0, "ITIMERS");
void
itimerslink(){
	init_hwtimer_itimers();
}
