/*
 * $FILE: time.c
 *
 * Time related functions
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

#include <time.h>

#define SECSPERMIN 60
#define MINSPERHOUR 60
#define HOURSPERDAY 24
#define SECSPERHOUR (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY ((long) SECSPERHOUR * HOURSPERDAY)
#define MONSPERYEAR 12
#define TM_YEAR_BASE 1900
#define EPOCH_YEAR 1970
#define EPOCH_LEAP 1968
#define BASE_TO_EPOCH_YEAR (EPOCH_YEAR - TM_YEAR_BASE)

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define leap_years_from_epoch(y) (((y) - EPOCH_LEAP - 1) / 4)

const static int days_to_month [MONSPERYEAR+1] =
{  0 /* Jan */,  31 /* Feb */,  59 /* Mar */,
  90 /* Apr */, 120 /* May */, 151 /* Jun */,
 181 /* Jul */, 212 /* Aug */, 243 /* Sep */,
 273 /* Oct */, 304 /* Nov */, 334 /*Dic */,
 365 /* end year */};

const static int days_to_month_leap [MONSPERYEAR+1] =
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
