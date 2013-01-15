/*
 * $FILE: integratorcp_pit.c
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
#include <arch/board.h>
#include <arch/io.h>
#include <arch/drivers.h>


#define PERIODIC_INTERVAL_NSEC 2000000000LL


#define CNT0 0x0
#define CNT1 0x100
#define CNT2 0x200

#define PIT_TIMERXLD(cnt) (PIT_BASE_ADDR + 0x0 + cnt)
#define PIT_TIMERXVAL(cnt) (PIT_BASE_ADDR + 0x4 + cnt)
#define PIT_TIMERXCTRL(cnt) (PIT_BASE_ADDR + 0x8 + cnt)
#define PIT_TIMERXINTCLR(cnt) (PIT_BASE_ADDR + 0xc + cnt)
#define PIT_TIMERXRIS(cnt) (PIT_BASE_ADDR + 0x10 + cnt)
#define PIT_TIMERXMIS(cnt) (PIT_BASE_ADDR + 0x14 + cnt)
#define PIT_TIMERXBGLOAD(cnt) (PIT_BASE_ADDR + 0x18 + cnt)

// Definitions
static timer_handler_t icp_timer_user_handler = NULL;
static hwtime_t bus_hz = 0;

static duration_t clock_ctime = 0;

//-----------------------//
// icp_timer_irq_handler //
//-----------------------//

static int icp_timer_irq_handler (context_t *context) {
  clock_ctime += PERIODIC_INTERVAL_NSEC;

  io_write (PIT_TIMERXINTCLR(CNT0), 0x1);

  // Another fix for QEMU
  // Cleaning Timer INT does not cleans the PIC interrupt
  // To work around the problem, we program a timer count without
  // interrupts, that will clear the PIC
  io_write (PIT_TIMERXCTRL(CNT0), 0x83);
  io_write (PIT_BASE_ADDR, 0x1);

  while (io_read(PIT_BASE_ADDR + 0x4));
  ////////////////////////////////////////


  // Timer set in periodic mode
  io_write (PIT_TIMERXCTRL(CNT0), 0xe2);
  //io_write (PIT_TIMERXLD(CNT0), 
  //(PERIODIC_INTERVAL_NSEC * bus_hz)/ NSECS_PER_SEC);
  io_write (PIT_TIMERXLD(CNT0), 0x2625a00);
  
  if (icp_timer_user_handler)
    (*icp_timer_user_handler) ();
  
  return 0;
}

//------------------------//
// init_hwtimer_icp_timer //
//------------------------//

static int init_hwtimer_icp_timer (void) {
  int cm_init;

  // Finding out the Bus frequency
  cm_init = io_read (CM_INIT);
  
  bus_hz = ((unsigned long)get_cpu_khz ()) * 1000/
    (((cm_init >> 4) & 0x7)+1);
  install_irq_handler_sys (ICP_TIMER0_IRQ, icp_timer_irq_handler);
  hw_enable_irq (ICP_TIMER0_IRQ);
  
  // Timer set in periodic mode
  io_write (PIT_TIMERXCTRL(CNT0), 0xe2);
  /*io_write (PIT_TIMERXLD(CNT0), 
    (PERIODIC_INTERVAL_NSEC * bus_hz)/ NSECS_PER_SEC);*/
  io_write (PIT_TIMERXLD(CNT0), 0x2625a00);

  return 0;
}

//-----------------------//
// set_hwtimer_icp_timer //
//-----------------------//

static void set_hwtimer_icp_timer (duration_t interval) {
  //int flags;
  //unsigned long cntr = (interval * bus_hz)/NSECS_PER_SEC;
  
  //hw_save_flags_and_cli (flags);
  // ENABLE | FREE_RUNNING | IE | NO_DIVISOR | 32b | ONESHOT

  //io_write (PIT_TIMERXCTRL(CNT0), 0xa3);
  //io_write (PIT_TIMERXLD(CNT0), cntr);

  //hw_restore_flags (flags);
}

//----------------------------//
// get_max_interval_icp_timer //
//----------------------------//

static duration_t get_max_interval_icp_timer (void) {
  return PERIODIC_INTERVAL_NSEC; /*(ICP_TIMER_ACCURATELY * NSECS_PER_SEC/ICP_TIMER_HZ);*/
}

//----------------------------//
// get_min_interval_icp_timer //
//----------------------------//

static duration_t get_min_interval_icp_timer (void) {
  return PERIODIC_INTERVAL_NSEC; // 50 usec
}

//-----------------------------//
// set_timer_handler_icp_timer //
//-----------------------------//

static timer_handler_t set_timer_handler_icp_timer (timer_handler_t 
						    timer_handler) {
  int flags;
  timer_handler_t old_icp_timer_user_handler = icp_timer_user_handler;

  hw_save_flags_and_cli (flags);
  icp_timer_user_handler = timer_handler;
  hw_restore_flags (flags);

  return old_icp_timer_user_handler;
}

//----------------------------//
// shutdown_hwtimer_icp_timer //
//----------------------------//

static void shutdown_hwtimer_icp_timer (void) {
  io_write (PIT_TIMERXCTRL(CNT0), 0x20);
  hw_disable_irq (ICP_TIMER0_IRQ);
  install_irq_handler_sys (ICP_TIMER0_IRQ, NULL);
}

hwtimer_t icp_timer = {
  init_hwtimer: init_hwtimer_icp_timer,
  set_hwtimer: set_hwtimer_icp_timer,
  get_max_interval: get_max_interval_icp_timer,
  get_min_interval: get_min_interval_icp_timer,
  set_timer_handler: set_timer_handler_icp_timer,
  shutdown_hwtimer: shutdown_hwtimer_icp_timer,
};

//----------------//
// init_icp_timer //
//----------------//

static int init_icp_timer (void) {
  return 0;
}

//----------------//
// read_icp_timer //
//----------------//

static hwtime_t read_icp_timer (void) {
  return clock_ctime;
}

//---------------------//
// read_icp_timer_nsec //
//---------------------//

static duration_t read_icp_timer_nsec (void) {
  return clock_ctime;
}

//---------------------------//
// hwtime2duration_icp_timer //
//---------------------------//

static duration_t hwtime2duration_icp_timer (hwtime_t hwtime) {
  return hwtime;
}

//------------------//
// getres_icp_timer //
//------------------//

static duration_t getres_icp_timer (void) {
  return PERIODIC_INTERVAL_NSEC; // 1 usec
}

//--------------------//
// shutdown_icp_timer //
//--------------------//

static void shutdown_icp_timer (void) {
}

system_clock_t icp_timer_clock = {
  init_clock: init_icp_timer,
  gettime_hwt: read_icp_timer,
  gettime_nsec: read_icp_timer_nsec,
  hwtime2duration: hwtime2duration_icp_timer,
  getres: getres_icp_timer,
  shutdown_clock: shutdown_icp_timer,
};

//REGISTER_DRV(init_hwtimer_icp_timer, 0, "INTEGRATORCP PIT");
//REGISTER_DRV(init_icp_timer, 0, "INTEGRATORCP CLOCK");
void
icp_pitlink(){
	init_icp_timer();
	init_hwtimer_icp_timer();
}
