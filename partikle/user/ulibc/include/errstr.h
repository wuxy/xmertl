/*
 * $FILE: errstr.h
 *
 * System error messages
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

#ifndef _ULIBC_ERRSTR_H_
#define _ULIBC_ERRSTR_H_

#include <config.h>
#include <errno.h>

#ifdef CONFIG_ULIBC_ERRSTR
extern const char *errstr[__SYS_NERR];
#else
extern const char *errstr[2];
#endif

extern const char  __sys_err_unknown [];

#endif
