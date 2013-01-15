/*
 * $FILE: assert.h
 *
 * Assertion facilities
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
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

#ifndef _ULIBC_ASSERT_H_
#define _ULIBC_ASSERT_H_

#include <stdlib.h>

BEGIN_DECLS

// no specified by STDC, is a GCC extension
#ifdef __PRETTY_FUNCTION__
#define __ASSERT_FUNCTION __PRETTY_FUNCTION__
#else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __ASSERT_FUNCTION	__func__
#  else
#   define __ASSERT_FUNCTION	__FUNCTION__
#  endif
#endif

#undef assert
#ifdef NDEBUG
#define assert(expr) ((void)0)
#else

/* This prints an "Assertion failed" message and aborts.  */
extern void _assert_fail (const char *assert, const char *file, unsigned int line, const char *func);

#ifdef expect
# define assert(expr) \
  ((void) (expect((long)(expr),0) ? 0 : \
	   (_assert_fail (#expr, \
			   __FILE__, __LINE__, __ASSERT_FUNCTION), 0)))
#else
# define assert(expr) \
  ((void) ((expr) ? 0 : \
	   (_assert_fail (#expr, \
			   __FILE__, __LINE__, __ASSERT_FUNCTION), 0)))
#endif
#endif

END_DECLS

#endif
