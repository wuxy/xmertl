/*
 * $FILE: heap.c
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

#include <domain.h>
#include <heap.h>

heap_t timer_heap;

//-----------//
// HEAP_init //
//-----------//

inline void HEAP_init (heap_t *heap) {
  heap -> nelem = 0;
}

//----------//
// HEAP_min //
//----------//

inline domain_t *HEAP_min (heap_t *heap) {
  if (heap -> nelem > 0) return heap -> array [1];

  return NULL;
}

//---------//
// heapify //
//---------//

static inline void heapify (heap_t *heap, int top) {
  domain_t *tempvalue = heap -> array [top];
  int child;

  for (child = top << 1; child <= heap -> nelem; top = child, child <<= 1) {
    if (child <= heap -> nelem && IS_MIN (heap -> array [child + 1],
                                         heap -> array [child]))
      child ++;

    if (IS_MIN (tempvalue,
                heap -> array [child]))
      break;

    heap -> array [top] = heap -> array [child];
    heap -> array [child] = tempvalue;
  }
}

/*
inline void heapfy (heap_t *heap, int i) {
  int l, r, min = i;
  domain_t *tmp;

  do {
    l = left (i);
    r = right (i);
    if (l < heap -> nelem && 
	IS_MIN (heap -> array [l], heap -> array [i]))
      min = l;
    if (r < heap -> nelem  && 
	IS_MIN (heap -> array [r], heap -> array [min]))
      min = r;

    if (min != i) {
      tmp = heap -> array [min];
      heap -> array [min] =  heap -> array [i];
      heap -> array [i] = tmp;
      min = i;
    }
  } while (min != i);
}
*/
//-----------------//
// HEAP_remove_min //
//-----------------//

inline domain_t *HEAP_remove_min (heap_t *heap){
  domain_t *min;
  if (heap -> nelem > 0) {
    min = heap -> array  [1];
    heap -> array [1] = heap -> array [heap -> nelem--];
    heapify (heap, 1);

    return min;
  }
  return NULL;
}

//---------------------//
// HEAP_remove_element //
//---------------------//

inline int HEAP_remove_element (heap_t *heap, domain_t *k) {
  int i;
  
  for (i = 1; i <= heap -> nelem; i ++)
    if (heap -> array [i] == k) break;

  if (heap -> array [i] != k) return -1;

  if (i <= heap -> nelem) {
    if (heap -> nelem - 1 > 0) {
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

//-------------//
// HEAP_insert //
//-------------//

inline int HEAP_insert (heap_t *heap, domain_t *k) {
  int i;
  
  if ((heap -> nelem + 1) > HEAP_MAX_NELEM) return -1;
  // A domain can not be inserted twice, so firstly we have to look for and
  // remove any other instace from the domain
  HEAP_remove_element (heap, k);

  i = ++heap -> nelem;
  
  while (i > 1 && IS_MIN (k, heap -> array [father (i)])) {
    heap -> array [i] = heap -> array [father(i)];
    i = father (i);
  }
  heap -> array [i] = k;

  return 0;
}

