/*
 * $FILE: pit.c
 *
 * pit_tsc driver
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
#include <arch/io.h>

// Definitions
#define PIT_IRQ 0
#define PIT_HZ 1193182
#define PIT_ACCURATELY 59659 // ~50 msec

static timer_handler_t pit_tsc_user_handler = NULL;

//-----------------//
// pit_irq_handler //
//-----------------//

static int pit_irq_handler (context_t *context) {

  if (pit_tsc_user_handler)
    (*pit_tsc_user_handler) ();

  hw_enable_irq (PIT_IRQ);
  return 0;
}

//----------------------//
// init_hwtimer_pit_tsc //
//----------------------//

static int init_hwtimer_pit (void) {
  install_irq_handler_sys (PIT_IRQ, pit_irq_handler);
  hw_enable_irq (PIT_IRQ);
  
  return 0;
}

//-----------------//
// set_hwtimer_pit //
//-----------------//

static void set_hwtimer_pit (duration_t interval) {
  int flags;
  unsigned long pit_counter = (interval * PIT_HZ) / NSECS_PER_SEC;
  
  hw_save_flags_and_cli (flags);
  // ONESHOOT_MODE
  outb(0x30, 0x43);
  outb (pit_counter & 0xff, 0x40);
  outb (pit_counter >> 8, 0x40);
  /* // PERIODIC MODE
  outb (0x30, 0x43);
  outb (0x34, 0x43);
  outb (pit_counter & 0xff, 0x40);
  outb (pit_counter >> 8, 0x40);
  }*/
  hw_restore_flags (flags);
}

//----------------------//
// get_max_interval_pit //
//----------------------//

static duration_t get_max_interval_pit (void) {
  return (PIT_ACCURATELY * NSECS_PER_SEC/PIT_HZ);
}

//----------------------//
// get_min_interval_pit //
//----------------------//

static duration_t get_min_interval_pit (void) {
  return 50000; // 50 usec
}

//-----------------------//
// set_timer_handler_pit //
//-----------------------//

static timer_handler_t set_timer_handler_pit (timer_handler_t 
					      timer_handler) {
  int flags;
  timer_handler_t old_pit_tsc_user_handler = pit_tsc_user_handler;
  hw_save_flags_and_cli (flags);
  pit_tsc_user_handler = timer_handler;
  hw_restore_flags (flags);
  return old_pit_tsc_user_handler;
}

//----------------------//
// shutdown_hwtimer_pit //
//----------------------//

static void shutdown_hwtimer_pit (void) {
  hw_disable_irq (PIT_IRQ);
  install_irq_handler_sys (PIT_IRQ, NULL);
}

hwtimer_t pit_tsc = {
  init_hwtimer: init_hwtimer_pit,
  set_hwtimer: set_hwtimer_pit,
  get_max_interval: get_max_interval_pit,
  get_min_interval: get_min_interval_pit,
  set_timer_handler: set_timer_handler_pit,
  shutdown_hwtimer: shutdown_hwtimer_pit,
};

//REGISTER_DRV(init_hwtimer_pit, 0, "PIT");
void
pitlink(void){
	init_hwtimer_pit();
}
