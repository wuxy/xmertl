/*
 * $FILE: time.c
 *
 * Time-related functions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *
  * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <time.h>
#include <syscalls.h>

#define SECSPERMIN 60
#define MINSPERHOUR 60
#define HOURSPERDAY 24
#define DAYSPERWEEK 7
#define DAYSPERNYEAR 365
#define DAYSPERLYEAR 366
#define SECSPERHOUR (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY ((long) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR 12

#define TM_SUNDAY 0
#define TM_MONDAY 1
#define TM_TUESDAY 2
#define TM_WEDNESDAY 3
#define TM_THURSDAY 4
#define TM_FRIDAY 5
#define TM_SATURDAY 6

#define TM_JANUARY 0
#define TM_FEBRUARY 1
#define TM_MARCH 2
#define TM_APRIL 3
#define TM_MAY 4
#define TM_JUNE 5
#define TM_JULY 6
#define TM_AUGUST 7
#define TM_SEPTEMBER 8
#define TM_OCTOBER 9
#define TM_NOVEMBER 10
#define TM_DECEMBER 11

#define TM_YEAR_BASE 1900

#define EPOCH_YEAR 1970
#define EPOCH_WDAY TM_THURSDAY

#define EPOCH_LEAP 1968

#define BASE_TO_EPOCH_YEAR (EPOCH_YEAR - TM_YEAR_BASE)

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define leap_years_from_epoch(y) (((y) - EPOCH_LEAP - 1) / 4)

const static int days_to_month[MONSPERYEAR+1] =
{  0 /* Jan */,  31 /* Feb */,  59 /* Mar */,
  90 /* Apr */, 120 /* May */, 151 /* Jun */,
 181 /* Jul */, 212 /* Aug */, 243 /* Sep */,
 273 /* Oct */, 304 /* Nov */, 334 /*Dic */,
 365 /* end year */};

const static int days_to_month_leap[MONSPERYEAR+1] =
{  0 /* Jan */,  31 /* Feb */,  60 /* Mar */,
  91 /* Apr */, 121 /* May */, 152 /* Jun */,
 182 /* Jul */, 213 /* Aug */, 244 /* Sep */,
 274 /* Oct */, 305 /* Nov */, 335 /*Dic */,
 366 /* end year */};

//--------//
// mktime //
//--------//

time_t mktime (struct tm *timeptr) {
  time_t days;
  
  days = 365 * (timeptr->tm_year - BASE_TO_EPOCH_YEAR);
  days += leap_years_from_epoch(timeptr->tm_year + TM_YEAR_BASE); 
  // leap years

  if (isleap(timeptr->tm_year + TM_YEAR_BASE))
    days += days_to_month_leap[timeptr->tm_mon];
  else
    days += days_to_month[timeptr->tm_mon];

  days += timeptr->tm_mday - 1;
  
  return days * SECSPERDAY + timeptr->tm_hour * SECSPERHOUR
    + timeptr->tm_min * SECSPERMIN + timeptr->tm_sec;
}

//-------------//
// localtime_r //
//-------------//

struct tm *localtime_r (const time_t *timep, struct tm *result) {
  int m;

  result->tm_mday = *timep / SECSPERDAY;
  result->tm_hour = *timep % SECSPERDAY / SECSPERHOUR;
  result->tm_min  = *timep % SECSPERHOUR / SECSPERMIN;
  result->tm_sec  = *timep % SECSPERMIN;
  result->tm_isdst = 0;
  result->tm_wday = (result->tm_mday + EPOCH_WDAY) % DAYSPERWEEK;

  result->tm_year = result->tm_mday / DAYSPERNYEAR;
  result->tm_year = (result->tm_mday -
                     leap_years_from_epoch(result->tm_year
                                           + EPOCH_YEAR)) / DAYSPERNYEAR;
  result->tm_mday = (result->tm_mday -
		     leap_years_from_epoch(result->tm_year
                                           + EPOCH_YEAR)) % DAYSPERNYEAR;

  result->tm_year += BASE_TO_EPOCH_YEAR;
  
  if (isleap(result->tm_year + TM_YEAR_BASE)) {
    for (m=TM_JANUARY; days_to_month_leap[m] <= result->tm_mday; m++);
    result->tm_mon = m - 1;
    result->tm_yday = result->tm_mday;
    result->tm_mday -= days_to_month_leap[m - 1];
    result->tm_mday += 1;
  } else {
    for (m=TM_JANUARY; days_to_month[m] <= result->tm_mday; m++);
    result->tm_mon = m - 1;
    result->tm_yday = result->tm_mday;
    result->tm_mday -= days_to_month[m - 1];
    result->tm_mday += 1;
  }
  
  return result;
}

//-----------//
// localtime //
//-----------//

struct tm *localtime(const time_t *timep) {
  static struct tm tms = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  
  return localtime_r (timep, &tms);
}

//--------//
// gmtime //
//--------//

struct tm *gmtime(const time_t *timep) {
  return localtime(timep);
}

//-----------//
// nanosleep //
//-----------//

_syscall2 (int, nanosleep, const struct timespec *, req, 
	   struct timespec *, rem);

//--------//
// usleep //
//--------//

_syscall1 (int, usleep, useconds_t, usec);

//---------------//
// clock_settime //
//---------------//

_syscall2 (int, clock_settime, clockid_t, clock_id, 
	   const struct timespec *,tp);

//---------------//
// clock_gettime //
//---------------//

_syscall2 (int, clock_gettime, clockid_t, clock_id, struct timespec *, tp);

//--------------//
// clock_getres //
//--------------//

_syscall2 (int, clock_getres, clockid_t, clock_id, struct timespec *, res);

//------//
// time //
//------//

_syscall1 (time_t, time, time_t *, t);

//--------------//
// gettimeofday //
//--------------//

_syscall2 (int, gettimeofday, struct timeval *, tv, struct timezone *, tz);

#include <signal.h>
//--------------//
// timer_create //
//--------------//

_syscall3 (int, timer_create, clockid_t, clockid, struct sigevent *, evp,
	   timer_t *, timerid);

//--------------//
// timer_delete //
//--------------//

_syscall1 (int, timer_delete, timer_t, timerid);

//---------------//
// timer_settime //
//---------------//

_syscall4 (int, timer_settime, timer_t, timerid, int, flags, 
	  const struct itimerspec *, value, 
	  struct itimerspec *, ovalue);

//---------------//
// timer_gettime //
//---------------//

_syscall2 (int, timer_gettime, timer_t, timerid, struct itimerspec *, value);

//------------------//
// timer_getoverrun //
//------------------//

_syscall1 (int, timer_getoverrun, timer_t, timerid);
