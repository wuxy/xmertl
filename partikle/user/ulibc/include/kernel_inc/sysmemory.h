/*
 * $FILE: sysmemory.h
 *
 * System memory manager
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

#ifndef _SYSMEMORY_H_
#define _SYSMEMORY_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <linkage.h>
#include <arch/sysmemory.h>

/*extern unsigned long init_vmm (void);
extern void *alloc_region (unsigned long, unsigned long, unsigned long);
extern void dealloc_region (void *, unsigned long);
extern void create_app_map (void);
*/

extern unsigned long init_sysmem(void); // It returns the amount of
					// free memory

/* Allocs an amount of memory in the user space */
extern asmlinkage void *ualloc_sys (int size);
/* Releases a previously allocated area of memory in user space */
extern asmlinkage void ufree_sys (void *ptr, int size); 
extern void create_app_map (void);

#endif
