/*
 * $FILE: pit.c
 *
 * pit driver
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

#include <hwtimer.h>
#include <linux_inc.h>
#include <heap.h>

// Definitions
#define PIT_IRQ 0
#define PIT_HZ 1193182
#define PIT_ACCURATELY 59659 // ~50 msec

//------------------//
// init_hwtimer_pit //
//------------------//

static int init_hwtimer_pit (void) {
  duration_t LINUX_NS;
  LINUX_NS = ((long long) LATCH * NSECS_PER_SEC)/CLOCK_TICK_RATE;
  HEAP_init (&timer_heap);
  
  sethwtimer (LINUX_NS, LINUX_NS);
  return 0;
}

//-----------------//
// set_hwtimer_pit //
//-----------------//

static void set_hwtimer_pit (duration_t interval) {
  unsigned long flags;
  unsigned long pit_counter = (interval * PIT_HZ) / NSECS_PER_SEC;
  
  hw_save_flags_and_cli (&flags);
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

//----------------------//
// shutdown_hwtimer_pit //
//----------------------//

static void shutdown_hwtimer_pit (void) {
  unsigned long flags;
  hw_save_flags_and_cli (&flags);
  outb_p (0x34,0x43);
  outb_p (LATCH & 0xff,0x40);
  outb (LATCH >> 8,0x40);
  hw_restore_flags (flags);
}

hwtimer_t hwtimer = {
  init_hwtimer: init_hwtimer_pit,
  set_hwtimer: set_hwtimer_pit,
  get_max_interval: get_max_interval_pit,
  get_min_interval: get_min_interval_pit,
  shutdown_hwtimer: shutdown_hwtimer_pit,
  timer_event: 0
};
