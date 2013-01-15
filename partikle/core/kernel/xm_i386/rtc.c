/*
 * $FILE: rtc.c
 *
 * RTC driver, XM version
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

#include <processor.h>
#include <time.h>

//---------------------//
// hwtime2duration_rtc //
//---------------------//

static duration_t hwtime2duration_rtc (hwtime_t hwtime) {
  return hwtime;
}

//---------------------//
// hwtime2duration_tsc //
//---------------------//

static inline duration_t hwtime2duration_tsc (hwtime_t hwtime) {
  hwtime_t s;
  s = hwtime / cpu_hz;
  return s * NSECS_PER_SEC +
    ((hwtime - s * cpu_hz) * NSECS_PER_SEC) / cpu_hz;
}

//------------//
// getres_rtc //
//------------//

static duration_t getres_rtc (void) {
  return 1/hwtime2duration_tsc (cpu_hz);
}

//-----------------------------//
// time_to_seconds_since_epoch //
//-----------------------------//

time_t time_to_seconds_since_epoch (void) {
  return 0;
}

//----------//
// init_rtc //
//----------//

static int init_rtc (void) {
  return 0;
}

#define rdtscll(val) \
  __asm__ __volatile__("rdtsc" : "=A" (val))

//---------------//
// read_tsc_nsec //
//---------------//

static inline duration_t read_tsc_nsec (void) {
  hwtime_t tsc_time, s;
  rdtscll (tsc_time);
  s = tsc_time / cpu_hz;
  return s * NSECS_PER_SEC +
    ((tsc_time - s * cpu_hz) * NSECS_PER_SEC) / cpu_hz;
}

//---------------//
// read_rtc_nsec //
//---------------//

static duration_t read_rtc_nsec (void) {
  return read_tsc_nsec ();
}

//--------------//
// shutdown_rtc //
//--------------//

static void shutdown_rtc (void) {
}

system_clock_t rtc_clock = {
  init_clock: init_rtc,
  gettime_hwt: read_rtc_nsec,
  gettime_nsec: read_rtc_nsec,
  hwtime2duration: hwtime2duration_rtc,
  getres: getres_rtc,
  shutdown_clock: shutdown_rtc,
};
system_clock_t* realtime_clock = &rtc_clock;

//REGISTER_DRV(init_rtc, "RTC");
void
rtclink(){
	init_rtc();
}
