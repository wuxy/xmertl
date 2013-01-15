/*
 * $FILE: mm.h
 *
 * XM's memory manager
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

#ifndef _XM_MM_H_
#define _XM_MM_H_

#include <arch/mm.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define PAGE_IN_USE 1
#define PAGE_FREE 0

#define is_page_in_use(flags) ((flags) & PAGE_IN_USE)
#define set_page_free(flags) ((flags) &= ~(PAGE_IN_USE))
#define set_page_in_use(flags) ((flags) |= PAGE_IN_USE)

#define get_page_type(flags) (((flags) >> 1) & 0xFF)
#define set_page_type(flags, type) (flags) |= ((type & 0xFF) << 1)

typedef struct mm_page_struct {
  unsigned long vaddr; // vaddr
  unsigned long maddr; // mapped address
  // flags bits
  // 0: free/in use
  // 1..8: usage given to the page
  int flags;
  struct mm_page_struct *prev, *next;
} mm_page_t;

static inline mm_page_t * init_page_list (void) {
  return NULL;
}

static inline void insert_page_list (mm_page_t *p, mm_page_t **l) {
  p -> prev = NULL;
  p -> next = (*l);
  if ((*l)) (*l) -> prev = p;
  (*l) = p;
}

static inline void remove_page_list (mm_page_t *p, mm_page_t **l) {
  if ((*l) == p) (*l) = p -> next;
  if (p -> prev) p -> prev -> next = p -> next;
  if (p -> next) p -> next -> prev = p -> prev;
}

extern unsigned long mm_alloc_page (void);
extern void mm_release_page (unsigned long pg);

#endif

