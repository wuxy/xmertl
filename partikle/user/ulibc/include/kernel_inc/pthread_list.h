/*
 * $FILE: pthread_list.h
 *
 * Pthreads list
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

#ifndef _PTHREAD_LIST_H_
#define _PTHREAD_LIST_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <stddef.h>

typedef struct pthread_list_struct {
  struct pthread_list_struct *prev, *next;
  pthread_t this;
} pthread_list_t;

//-------------------//
// init_pthread_list //
//-------------------//

static inline void init_pthread_list (pthread_list_t **h) {
  *h = NULL;
}

//---------------------//
// insert_pthread_list //
//---------------------//

static inline void insert_pthread_list (pthread_list_t **h, 
					pthread_list_t *p) {
  if (*h)
    (*h) -> prev = p;
  
  p -> next = *h;
  *h = p;
}

//---------------------//
// remove_pthread_list //
//---------------------//

static inline void remove_pthread_list (pthread_list_t **h, 
					pthread_list_t *p) {
  if (p -> next)
    p -> next -> prev = p -> prev;

  if (p -> prev)
    p -> prev -> next = p -> next;
  
  if (*h == p)
    *h = p -> next;

  p -> prev = p -> next = NULL;
}

//--------------------------//
// remove_head_pthread_list //
//--------------------------//

static inline pthread_t remove_head_pthread_list (pthread_list_t **h) {
  pthread_list_t *p = *h;
  pthread_t t = NULL;

  if (*h)
    *h = (*h) -> next;

  if (*h)
    (*h) -> prev = NULL;

 if (p) {
   p -> prev = p -> next = NULL;
   t = p -> this;
 }

  return t;
}

#endif
