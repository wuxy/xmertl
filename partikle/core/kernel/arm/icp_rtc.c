/*
 * $FILE: icp_rtc.c
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
#include <drivers.h>
#include <time.h>
#include <processor.h>

// Definitions

//--------------//
// init_icp_rtc //
//--------------//

static int init_icp_rtc (void) {
  return 0;
}

//--------------//
// read_icp_rtc //
//--------------//

static hwtime_t read_icp_rtc (void) {
  hwtime_t icp_rtc_time;
  rdicp_rtcll (icp_rtc_time);
  return icp_rtc_time;
}

//---------------//
// read_icp_rtc_nsec //
//---------------//

static duration_t read_icp_rtc_nsec (void) {
  hwtime_t icp_rtc_time, s;
  rdicp_rtcll (icp_rtc_time);
  s = icp_rtc_time / cpu_hz;
  return s * NSECS_PER_SEC + 
    ((icp_rtc_time - s * cpu_hz) * NSECS_PER_SEC) / cpu_hz;
}

//---------------------//
// hwtime2duration_icp_rtc //
//---------------------//

static duration_t hwtime2duration_icp_rtc (hwtime_t hwtime) {
  hwtime_t s;
  s = hwtime / cpu_hz;
  return s * NSECS_PER_SEC + 
    ((hwtime - s * cpu_hz) * NSECS_PER_SEC) / cpu_hz;
}

//------------//
// getres_icp_rtc //
//------------//

static duration_t getres_icp_rtc (void) {
  return 1/hwtime2duration_icp_rtc (cpu_hz);
}

//--------------//
// shutdown_icp_rtc //
//--------------//

static void shutdown_icp (void) {
}

system_clock_t monotonic_clock = {
  init_clock: init_icp_rtc,
  gettime_hwt: read_icp_rtc,
  gettime_nsec: read_icp_rtc_nsec,
  hwtime2duration: hwtime2duration_icp_rtc,
  getres: getres_icp_rtc,
  shutdown_clock: shutdown_icp_rtc,
};

REGISTER_DRV(init_icp, 0, "INTEGRATORCP RTC");
