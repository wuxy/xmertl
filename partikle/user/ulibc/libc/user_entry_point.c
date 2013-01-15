/*
 * $FILE: user_entry_point.c
 *
 * This file implements the entry point
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

#include <arch/entry_point.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <config.h>
#include <sys/mman.h>
#include <kernel_inc/tlsf.h>

char *_dyn_mem_pool = (char *)0xFFFFFFFF;
extern void init_lang_supp (void);

//------------//
// init_ulibc //
//------------//

static void init_ulibc (void) {
  if (!(_dyn_mem_pool = (char *) ualloc (CONFIG_UDYNAMIC_MEMORY)))
    _exit (-1);  
  if (!init_memory_pool (CONFIG_UDYNAMIC_MEMORY, _dyn_mem_pool))
    _exit (-1);
  init_lang_supp ();
}

ENTRY_POINT ();
