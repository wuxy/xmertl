/*
 * $FILE: sysmem.h
 *
 * System memory map, basically it is implemented as a first-fit
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

#ifndef _ARCH_SYSMEM_H_
#define _ARCH_SYSMEM_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

struct free_ptr {
  struct head_struct *next, *prev;
};

#define USED_BLOCK 0x80000000
#define FREE_BLOCK ~USED_BLOCK //0x7FFFFFFF

#define IS_USED_BLOCK(x) ((x -> size & USED_BLOCK) == USED_BLOCK)
#define GET_BLOCK_SIZE(x) (x -> size & FREE_BLOCK)
#define SET_USED_BLOCK(x) (x -> size |= USED_BLOCK)
#define SET_FREE_BLOCK(x) (x -> size &= FREE_BLOCK)

typedef struct head_struct {
  unsigned int size;

  struct head_struct *prev_phys, *next_phys;
  union mem {
    struct free_ptr free_ptr;
    unsigned char ptr[sizeof (struct free_ptr)];
  } mem;
} list_t;

typedef struct list_header_struct {
  list_t *head;
} list_header_t;

#endif
