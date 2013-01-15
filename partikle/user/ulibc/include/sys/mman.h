/*
 * $FILE: mman.h
 *
 * Memory management
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

#ifndef _ULIBC_MMAN_H_
#define _ULIBC_MMAN_H_

#include <sys/types.h>


#define MAP_PRIVATE   0 
#define MAP_ANONYMOUS 1

extern void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);

extern int munmap(void *start, size_t length);

// Non standard way of getting free memory

extern void *ualloc (int size);
extern void ufree (void *ptr, int size);

#endif
