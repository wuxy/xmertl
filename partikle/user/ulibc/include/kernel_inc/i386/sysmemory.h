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

#ifndef _ARCH_SYSMEMORY_H_
#define _ARCH_SYSMEMORY_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <config.h>

// Page flags
#define DMA_PAGE 0x1

#define USER_PAGE 0x2
#define KERNEL_PAGE 0x4

//extern unsigned long alloc_phys_page (void);
//extern void dealloc_phys_page (unsigned long);
#ifdef CONFIG_X86_PAGING
extern void *alloc_region (unsigned long from, unsigned long to, 
			   unsigned long size);
extern void dealloc_region (void *saddr, unsigned long size);
extern unsigned long alloc_phys_page (void);
extern void dealloc_phys_page (unsigned long addr);

#endif

#endif
