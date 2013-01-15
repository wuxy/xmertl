/*
 * $FILE: fcntl.h
 *
 * File control options
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

#ifndef _ULIBC_FCNTL_H_
#define _ULIBC_FCNTL_H_
#include <kernel_inc/compiler.h>

BEGIN_DECLS

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_NONBLOCK 4

// Dummy values
#define O_CREAT 0
#define O_APPEND 0
#define O_TRUNC 0
#define O_EXCL 0

extern int open (const char *, int, int);

END_DECLS

#endif
