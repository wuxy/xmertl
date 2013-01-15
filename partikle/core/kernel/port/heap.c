/* 
 * $FILE: heap.c
 *
 * A simple but efficient heap implementation, it
 * is used by the virtual timers
 *
 * $VERSION: 1.0
 *
 * Miguel Masmano <mimastel@doctor.upv.es>
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

#include <heap.h>
#include <string.h>
#include <stdio.h>

//-----------//
// HEAP_init //
//-----------//

inline void HEAP_init (heap_t *heap) {
  heap -> nelem = 0;
  memset ((unsigned char *) &heap -> array[0], 0, 
	  (HEAP_MAX_NELEM + 1) * sizeof (T));
}

//----------//
// HEAP_min //
//----------//

inline T HEAP_min (heap_t *heap) {
  if (heap -> nelem > 0)
    return heap -> array [1];
  
  return NULL;
}

//---------//
// heapify //
//---------//

static inline void heapify (heap_t *heap, int top) {
  T tempvalue = heap -> array [top];
  int child;

  for (child = top << 1; child <= heap -> nelem; top = child, child <<= 1) {
	  if (child<=heap->nelem&&(child+1)<=heap->nelem&&
		  IS_MIN(heap->array[child+1], heap->array[child]))
      child ++;

    if (IS_MIN (tempvalue, 
		heap -> array [child]))
      break;
    
    heap -> array [top] = heap -> array [child];
    heap -> array [child] = tempvalue;
  }
}

//-----------------//
// HEAP_remove_min //
//-----------------//

inline T HEAP_remove_min (heap_t *heap){
  T min;

  if (heap -> nelem > 0) {
    min = heap -> array  [1];
    heap -> array [1] = heap -> array [heap->nelem--];
    heapify (heap, 1);

    return min;
  }

  return NULL;
}

//-------------//
// HEAP_insert //
//-------------//

inline int HEAP_insert (heap_t *heap, const T k) {
  int i;

  if ((heap -> nelem + 1) > HEAP_MAX_NELEM) return -1;

  i = ++heap -> nelem;
  
  while (i > 1 && IS_MIN (k, heap -> array [father (i)])) {
    heap -> array [i] = heap -> array [father(i)];
    i = father (i);
  }
  
  heap -> array [i] = (T) k;

  return 0;
}


//---------------------//
// HEAP_remove_element //
//---------------------//

inline int HEAP_remove_element (heap_t *heap, const T k) {
  int i;

  for (i = 1; i <= heap -> nelem; i ++)
    if (heap -> array [i] == k) break;
  if (i <= heap -> nelem) {
    if (heap -> nelem > 1) {
      heap -> array [i] = heap -> array [heap -> nelem--];
      heapify (heap, i);
    } else {
      heap -> nelem = 0;
      heap -> array [i] = NULL;
    }

    return 0;
  }

  return -1;
}

//----------------//
// HEAP_check_min //
//----------------//

inline int HEAP_check_min (heap_t *heap) {
  int n;
  for (n = 2; n <= heap -> nelem; n++)
    if (!IS_MIN (heap -> array [1], heap -> array [n])) return 0;

  return 1;
}

//------------//
// HEAP_print //
//------------//

inline void HEAP_print (heap_t *heap) {
  int n;

  printf ("HEAP ELEMENTS ============\n");
  for (n = 1; n <= heap -> nelem; n++)
  printf ("[%d] timer: 0x%x expiration: %lld\n", n, heap -> array [n],
    heap -> array [n]-> timer_value);

  printf ("End HEAP ==================\n");
}

