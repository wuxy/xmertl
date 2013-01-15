/*
 * $FILE: vmm.c
 *
 * Virtual memory manager
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
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

#include <config.h>
#include <sysmemory.h>
#include <stddef.h>
#include <cbitmap.h>
#include <processor.h>
#include <tlsf.h>
//#include <arch/board.h>
//#include <arch/io.h>

#include "mem.h"
#include "lib.h"


//----------//
// init_mem //
//----------//

static unsigned long init_mem (void) {
  return (RAMTOP - (ulong) ebss);
}

//--------------//
// alloc_region //
//--------------//

static void *alloc_region (unsigned long from, unsigned long to, 
			   unsigned long size) {

  if ((ulong)ebss <= from && to <= RAMTOP && (size <= RAMTOP - (ulong)ebss))
	  return (void *) ebss;
  return NULL;
}

//----------------//
// dealloc_region //
//----------------//

//static void dealloc_region (void *saddr, unsigned long size) {
//}

//------------//
// ualloc_sys //
//------------//

asmlinkage void *ualloc_sys (int size) {
  return malloc_ex (size, memory_pool);
}

//-----------//
// ufree_sys //
//-----------//

asmlinkage void ufree_sys (void *ptr, int size) {
  free_ex (ptr, memory_pool);
}

//----------------//
// create_app_map //
//----------------//

void create_app_map (void) {
}

//-------------//
// init_sysmem //
//-------------//

unsigned long init_sysmem (void) {

  unsigned long freemem = init_mem ();
  
  if (CONFIG_KDYNAMIC_MEMORY > freemem) return -1;

  if (!(memory_pool = alloc_region
	((ulong)ebss, RAMTOP, CONFIG_KDYNAMIC_MEMORY))) return -1;

  // Setting up TLSF with the largest free area, memory_pool will be
  // zeroed by this function as well
  printf ("\nSetting up the dynamic memory manager (%d kbytes at 0x%x)\n", 
	  CONFIG_KDYNAMIC_MEMORY/1024, memory_pool);

  if (init_memory_pool (CONFIG_KDYNAMIC_MEMORY, memory_pool) == 0) return -1;
  
  return freemem;
}
