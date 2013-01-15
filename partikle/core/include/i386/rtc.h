/*
 * $FILE: rtc.h
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

#ifndef _RTC_H_
#define _RTC_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

/*
 * RTC Ports
 */

#define IO_RTC 0x070 /* RTC */

#define RTC_SEC 0x00 /* seconds */
#define RTC_MIN 0x02 /* minutes */
#define RTC_HRS 0x04 /* hours */
#define RTC_DAY 0x07 /* day of month */
#define RTC_MONTH 0x08 /* month of year */
#define RTC_YEAR 0x09 /* month of year */

#define RTC_STATUSA 0x0a /* status register A */
#define RTCSA_TUP 0x80 /* time update, don't look now */
#define RTC_STATUSB 0x0b /* status register B */
#define RTC_INTR 0x0c /* status register C (R) interrupt source */
#define RTC_STATUSD 0x0d /* status register D (R) Lost Power */

struct rtc_time {
  unsigned char year;  // year two last digits (1999 => 99 and 2000 => 0)
  unsigned char month; // range [1,12]
  unsigned char day;   // range [1,31]
  unsigned char hour;  // range [0,23]
  unsigned char min;   // range [0,59]
  unsigned char sec;   // range [0,59]
};

#endif
