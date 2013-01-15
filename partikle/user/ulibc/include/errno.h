/*
 * $FILE: errno.h
 *
 * System error numbers
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

#ifndef _ULIBC_ERRNO_H_
#define _ULIBC_ERRNO_H_

#include <config.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

extern int _get_errno (void);
#define errno _get_errno()

extern void _set_errno (int);

#define SET_ERRNO(errno) _set_errno (errno)

#include <kernel_inc/errno.h>

END_DECLS

#endif
