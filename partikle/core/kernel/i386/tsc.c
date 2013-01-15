/*
 * $FILE: tsc.c
 *
 * TSC driver
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
#include <time.h>
#include <processor.h>

// Definitions

//----------//
// init_tsc //
//----------//

static int init_tsc (void) {
  return 0;
}

//----------//
// read_tsc //
//----------//

#define rdtscll(val) \
  __asm__ __volatile__("rdtsc" : "=A" (val))

static hwtime_t read_tsc (void) {
  hwtime_t tsc_time;
  rdtscll (tsc_time);
  return tsc_time;
}

//---------------//
// read_tsc_nsec //
//---------------//

static duration_t read_tsc_nsec (void) {
  hwtime_t tsc_time, s;
  rdtscll (tsc_time);
  s = tsc_time / cpu_hz;
  return s * NSECS_PER_SEC + 
    ((tsc_time - s * cpu_hz) * NSECS_PER_SEC) / cpu_hz;
}

//---------------------//
// hwtime2duration_tsc //
//---------------------//

static duration_t hwtime2duration_tsc (hwtime_t hwtime) {
  hwtime_t s;
  s = hwtime / cpu_hz;
  return s * NSECS_PER_SEC + 
    ((hwtime - s * cpu_hz) * NSECS_PER_SEC) / cpu_hz;
}

//------------//
// getres_tsc //
//------------//

static duration_t getres_tsc (void) {
  return 1/hwtime2duration_tsc (cpu_hz);
}

//--------------//
// shutdown_tsc //
//--------------//

static void shutdown_tsc (void) {
}

system_clock_t tsc_clock = {
  init_clock: init_tsc,
  gettime_hwt: read_tsc,
  gettime_nsec: read_tsc_nsec,
  hwtime2duration: hwtime2duration_tsc,
  getres: getres_tsc,
  shutdown_clock: shutdown_tsc,
};

//REGISTER_DRV(init_tsc, 0, "TSC");
void
tsclink(void){
	init_tsc();
}
