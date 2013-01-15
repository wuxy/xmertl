/*
 * $FILE: arch_types.h
 *
 * Types defined by the architecture
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

#ifndef _ARCH_TYPES_H_
#define _ARCH_TYPES_H_

typedef long long hwtime_t;

#ifdef _KERNEL_
typedef unsigned long *addr_t;
#define BITS_PER_WORD 32
#endif

typedef long long duration_t;
typedef int time_t;

typedef long ptrdiff_t;
#ifndef __cplusplus
typedef unsigned int wchar_t;
#endif
typedef unsigned int size_t;
typedef int ssize_t;
typedef long loff_t;
typedef long off_t;
typedef char *caddr_t;
typedef long suseconds_t;
typedef unsigned long useconds_t;

#endif
