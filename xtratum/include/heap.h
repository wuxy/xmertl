/*
 * $FILE: heap.h
 *
 * A heap implementation
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

#ifndef _XM_HEAP_H_
#define _XM_HEAP_H_
#include <domain.h>

#define HEAP_MAX_NELEM 100

#define father(i) ((int)(i/2))
#define right(i) (2 * i + 1)
#define left(i) (2 * i)

#define IS_MIN(n1, n2) (n1 -> time.hwtime < n2 -> time.hwtime)

typedef struct heap_struct {
  struct domain_struct *array [HEAP_MAX_NELEM];
  int nelem;
} heap_t;

extern void HEAP_init (heap_t *heap);
extern domain_t *HEAP_min (heap_t *heap);
extern void heapfy (heap_t *heap, int i);
extern domain_t *HEAP_remove_min (heap_t *heap);
extern int HEAP_insert (heap_t *heap, domain_t *k);
extern int HEAP_remove_element (heap_t *heap, domain_t *k);
extern void HEAP_show_elements (heap_t *heap);
extern heap_t timer_heap;

#endif
