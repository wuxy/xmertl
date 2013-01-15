/*
 * $FILE: sched_struct.h
 *
 * Structure used by the scheduler
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

#ifndef _SCHED_STRUCT_H_
#define _SCHED_STRUCT_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <bitmap.h>
#include <stddef.h>

// Basically, it is composed by a 1024 lists of threads, where each
// list represents a different priority, that is, list 0 is priority 0
// and so on, to speed up the search in the structure, we use
// bitmaps, where each bit represents the state of a list, only
// ready threads are inserted in this structure

// Max priority 0
// Min priority 1023

typedef struct active_threads_struct {
  // There are a maximum of 1024 priorities
  pthread_t thread_matrix [32][32]; // There are a maximum of 1024 priorities
  // First level bitmap
  bitmap_t flbitmap;
  // Second level bitmap
  bitmap_t slbitmap [32];
} active_threads_t;

//---------//
// prio2ij //
//---------//

// prio must be less than 1023
static inline void prio2ij (int prio, int *i, int *j) {
  *i = (prio & 0x3ff) >> 5;
  *j = (prio & 0x1f);
}

//---------//
// ij2prio //
//---------//

static inline void ij2prio (int *prio, int i, int j) {
  *prio = ((i & 0x1f) << 5) | (j & 0x1f);
}

//---------//
// init_ss //
//---------//

static inline void init_ss (active_threads_t *active_threads) {
  int cont1, cont2;
  for (cont1 = 0; cont1 < 32; cont1 ++) {
    active_threads->slbitmap [cont1] = clearbitmap ();
    for (cont2 = 0; cont2 < 32; cont2 ++)
      active_threads->thread_matrix [cont1][cont2] = NULL;
  }
  active_threads->flbitmap = clearbitmap ();
}

//------------//
// lookfor_ss //
//------------//

/*
static int lookfor_ss (pthread_t thread,
		       active_threads_t *active_threads) {
  int i, j;
  pthread_t p;

  for (i = 0; i < 32; i++)
    for (j = 0; j < 32; j++) {
      p = active_threads->thread_matrix [i][j];
      while (p) {
	if (p == thread) {
	  return 1;
	}
	p = p -> next_matrix;
      }
    }
  return 0;
}
*/

//-----------//
// insert_ss //
//-----------//

static inline void insert_ss (pthread_t thread, int priority,
			      active_threads_t *active_threads) {
  int i, j;
  
  thread -> next_matrix = thread -> prev_matrix = NULL;
  prio2ij (priority, &i, &j);
  thread -> next_matrix = active_threads->thread_matrix [i][j];
  if (active_threads->thread_matrix [i][j]) 
    active_threads->thread_matrix [i] [j] -> prev_matrix = thread;

  active_threads->thread_matrix [i][j] = thread;
  active_threads->flbitmap = setbit (active_threads->flbitmap, i);
  active_threads->slbitmap[i] = setbit (active_threads->slbitmap[i], j);
  thread -> ss_i = i; 
  thread -> ss_j = j;
}

//----------------//
// insert_ss_tail //
//----------------//

static inline void insert_ss_tail (pthread_t thread, int priority,
				   active_threads_t *active_threads) {
  int i, j;
  pthread_t aux; 

  prio2ij (priority, &i, &j);

  thread -> next_matrix = thread -> prev_matrix = NULL;
  
  if (active_threads -> thread_matrix [i][j]) {
    aux = active_threads->thread_matrix [i][j];
  
    // Aux'll be the tail
    while (aux -> next_matrix)
      aux = aux -> next_matrix;

    aux -> next_matrix = thread;
    thread -> prev_matrix = aux;
  } else {
    active_threads -> thread_matrix [i][j] = thread;
  }

  active_threads->flbitmap = setbit (active_threads->flbitmap, i);
  active_threads->slbitmap[i] = setbit (active_threads->slbitmap[i], j);
  thread -> ss_i = i; 
  thread -> ss_j = j;
}

//-------------//
// get_head_ss //
//-------------//

// This function removes the head of a list
/*
static inline pthread_t get_head_ss (int priority, 
				      active_threads_t *active_threads) {
  int i, j;
  pthread_t aux;
  prio2ij (priority, &i, &j);
  
  // The correspondent list is empty
  if (!isbitset (active_threads->flbitmap, i) || 
      !isbitset (active_threads->slbitmap[i], j)) 
    return NULL;

  // Aux is the head of the list
  aux = active_threads->thread_matrix[i][j];
  // Removing aux from the list
  active_threads->thread_matrix [i][j] = aux -> next_matrix;
  aux -> next_matrix = NULL;

  if (active_threads->thread_matrix [i][j])
    // If there are any block, it is not necessary to update the
    // bitmaps, just the pointers of this block
    active_threads->thread_matrix [i][j] -> prev_matrix = NULL;
  else {
    // Updating bitmaps
    active_threads->slbitmap[i] = clearbit (active_threads->slbitmap[i], j);
    if (!active_threads->slbitmap[i])
      active_threads->flbitmap = clearbit (active_threads->flbitmap, i);
  }
  
  return NULL;
}
*/

static inline pthread_t get_head_ss (int *priority, 
				     active_threads_t *active_threads) {
  int i, j;
  pthread_t aux;

  if (!active_threads->flbitmap) {
    if (priority)
      *priority = - 1;
    return NULL;
  }
  
  i = _ffs (active_threads->flbitmap);
  j = _ffs (active_threads->slbitmap [i]);

  if (priority)
    ij2prio (priority, i, j);
  
  // Aux is the head of the list
  aux = active_threads->thread_matrix[i][j];

  // Removing aux from the list
  /*active_threads->thread_matrix [i][j] = aux -> next_matrix;
    aux -> next_matrix = NULL;
    
    if (active_threads->thread_matrix [i][j])
    // If there are any block, it is not necessary to update the
    // bitmaps, just the pointers of this block
    active_threads->thread_matrix [i][j] -> prev_matrix = NULL;
    else {
    // Updating bitmaps
    active_threads->slbitmap[i] = clearbit (active_threads->slbitmap[i], j);
    if (!active_threads->slbitmap[i])
    active_threads->flbitmap = clearbit (active_threads->flbitmap, i);
    }*/
  
  return aux;
}

/*
#include <stdio.h>
int _i, _j;
pthread_t _p;
static inline void check_ss (active_threads_t *active_threads) {
  printf ("++++++++++++\n");
  for (_i = 0; _i < 32; _i++)
    for (_j = 0; _j < 32; _j++) {
      //printf ("i, j: %d, %d\n", i, j);
      _p = active_threads->thread_matrix [_i][_j];
      while (_p) {
	printf ("thread: 0x%x\n", _p);
	_p = _p -> next_matrix;
      }
    }
  printf ("<><><><>\n");
}
*/

//-----------//
// remove_ss //
//-----------//

static inline void remove_ss (pthread_t thread,
			      active_threads_t *active_threads) {
  int i = thread -> ss_i, j = thread -> ss_j;
  if (i == -1 || j == -1) return;

  if (thread -> prev_matrix)
    thread -> prev_matrix -> next_matrix = thread -> next_matrix;
  
  if (thread -> next_matrix)
    thread -> next_matrix -> prev_matrix = thread -> prev_matrix;
  
  if (active_threads->thread_matrix [i][j] == thread)
    active_threads->thread_matrix [i][j] = thread -> next_matrix;
  
  thread -> next_matrix = thread -> prev_matrix = NULL;
  if (!active_threads->thread_matrix [i][j]) {
    // Updating bitmaps
    active_threads->slbitmap[i] = clearbit (active_threads->slbitmap[i], j);
    if (!active_threads->slbitmap[i])
      active_threads->flbitmap = clearbit (active_threads->flbitmap, i);
  }

  thread -> ss_i = thread -> ss_j = -1;
}

#endif
