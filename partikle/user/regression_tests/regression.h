/*
 * $FILE: regression.h
 *
 * Regression tests
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

#ifndef _REGRESSION_TESTS_
#define _REGRESSION_TESTS_

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define TEST_OK   0
#define TEST_FAIL 1

static inline void 
eprintf (char * fmt, ...) {
  va_list args;

  va_start(args, fmt);
  printf(fmt, args);
  va_end(args);

  if (fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':')
    printf(" %s", strerror(errno));

  printf("\n");
  exit(TEST_FAIL);
}

#endif
