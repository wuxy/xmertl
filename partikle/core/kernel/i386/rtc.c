/*
 * $FILE: rtc.c
 *
 * RTC driver
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
#include <arch/io.h>
#include <arch/rtc.h>

static duration_t rtc_offset = 0;

//----------//
// read_rtc //
//----------//

static inline unsigned char read_rtc (unsigned char val) {
  unsigned char r;
  outb_p (IO_RTC, val);
  r = inb_p (IO_RTC + 1);
  return r;
}

//---------------//
// read_rtc_time //
//---------------//

static inline void read_rtc_time (struct rtc_time *time) {
  int flags;
  
  //while (read_rtc (RTC_STATUSA) & RTCSA_TUP); // wait for update end
  
  hw_save_flags_and_cli (flags);
  time -> year  = read_rtc (RTC_YEAR);
  time -> month = read_rtc (RTC_MONTH);
  time -> day   = read_rtc (RTC_DAY);
  time -> hour  = read_rtc (RTC_HRS);
  time -> min   = read_rtc (RTC_MIN);
  time -> sec   = read_rtc (RTC_SEC);
  hw_restore_flags (flags);

#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)

  BCD_TO_BIN (time->year);
  BCD_TO_BIN (time->month);
  BCD_TO_BIN (time->day);
  BCD_TO_BIN (time->hour);
  BCD_TO_BIN (time->min);
  BCD_TO_BIN (time->sec);

#undef BCD_TO_BIN
}

//----------------------------------//
//  rtc_time_to_seconds_since_epoch //
//----------------------------------//

time_t time_to_seconds_since_epoch (void) {
  struct rtc_time t;
  struct tm stm;
  
  read_rtc_time (&t);

  stm.tm_year = (t.year < 70)? t.year + 100: t.year;
  // 'stm.tm_year' stores the number of years since 1900
  stm.tm_mon  = t.month - 1;
  // In 'struct tm' the months are numbered from 0 to 11
  stm.tm_mday = t.day;
  stm.tm_hour = t.hour;
  stm.tm_min  = t.min;
  stm.tm_sec  = t.sec;

  return mktime (&stm);
}

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

//----------//
// init_rtc //
//----------//

static int init_rtc (void) {
  rtc_offset = time_to_seconds_since_epoch () * NSECS_PER_SEC;
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
  return read_tsc_nsec () + rtc_offset;
}

//--------------//
// shutdown_rtc //
//--------------//

static void shutdown_rtc (void) {
}

system_clock_t rtc_clock = {
  init_clock: init_rtc,
  gettime_hwt:  read_rtc_nsec,
  gettime_nsec:  read_rtc_nsec,
  hwtime2duration: hwtime2duration_rtc,
  getres: getres_rtc,
  shutdown_clock: shutdown_rtc,
};

//REGISTER_DRV(init_rtc, 0, "RTC");
void
rtclink(void){
	init_rtc();
}
