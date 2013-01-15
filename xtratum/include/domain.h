/*
 * $FILE: domain.h
 *
 * Domains' routines
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

#ifndef _XM_DOMAIN_H_
#define _XM_DOMAIN_H_

#ifdef UXMLIB
#include <xm/arch/domain.h>

//typedef void (*handler_t) (int, void *);
#endif

#ifndef UXMLIB

struct domain_struct;

#include <config.h>
//#include <events.h>
#include <irqs.h>
#include <linux_inc.h>
#include <bitmap.h>
#include <timer.h>
#include <arch/domain.h>
#include <mm.h>
#include <events.h>

//typedef void (*handler_t) (int, struct pt_regs *);

typedef struct domain_struct {
  // Supervisor stack
  unsigned long *sstack; /* HARDCODED, don't change it */
  // Domain's page directory
  unsigned long pd; /* HARDCODED, don't change it */
  unsigned long *sstack_st;

  // User stack
  //unsigned long *ustack;
  //unsigned long *ustack_st;

  // Supervisor stack when the irq was triggered
  //unsigned long *irq_sstack;

  // Domain's name
  char *name;
  // Domain's id number
  int id;
  // Domain's priority
  unsigned int priority;

  unsigned long flags;
  event_handling_t *events;
  unsigned long events_user_addr;
  
  time_inf_t time;

  unsigned long *heap;

  mm_page_t *mm_page_list;
  
  struct domain_struct *prev, *next;
} domain_t;


/////////////////////////
// Domain flag's bits: //
/////////////////////////

// 4 : indicates whether the current domain can receive events or not
// 3..0 : state (4 bits, 16 possible states)

////////////////////// POSSIBLE STATES OF A DOMAIN

#define STATE_MASK 0xF // 4 bits (3..0)
#define DOMAIN_ACTIVE 0x0
#define DOMAIN_SUSPENDED 0x1
#define DOMAIN_FINISHED 0x2

#define set_domain_state(d, state) \
  (d) -> flags = (((d) -> flags & ~(STATE_MASK)) | state)

#define get_domain_state(d) \
  ((d) -> flags & STATE_MASK)

///////////////////// EVENTS ENABLED

#define enable_events_flag(d) \
  (d) -> events -> event_flags |= EVENTS_ENABLED

#define disable_events_flag(d) \
  (d) -> events -> event_flags &= ~(EVENTS_ENABLED)

#define is_events_flag_enabled(d) \
  ((d) -> events -> event_flags & EVENTS_ENABLED)

extern domain_t *create_domain (char *, unsigned long, 
				int (*) (void));

extern void destroy_domain (domain_t *d);

extern domain_t *__create_domain_node (char *, unsigned long);

extern void __remove_domain_node (domain_t *);

extern domain_t *xm_current_domain, *xm_root_domain, *xm_domain_list;

static inline domain_t *search_domain_id_list (int id, domain_t **list) {
  domain_t *p1;

  p1 = (*list);
  while (p1) {
    if (p1 -> id == id)
      break;
    p1 = p1 -> next;
  }
  
  return p1;
}

static inline void insert_domain_list (domain_t *node, domain_t **list) {
  domain_t *p1;

  if (!(*list)) {
    (*list) = node;
    node -> prev = node -> next = NULL;
  } else {
    p1 = (*list);
    while (p1 -> next) {
      if (node -> priority < p1 -> priority) break;
      p1 = p1 -> next;
    }

    if (node -> priority < p1 -> priority) {
      node -> next = p1;
      node -> prev = p1 -> prev;
      if (p1 -> prev) p1 -> prev -> next = node;
      p1 -> prev = node;
      if ((*list) == p1) (*list) = node;
    } else {
      node -> prev = p1;
      node -> next = p1 -> next;
      // This condition will never be reached
      if (p1 -> next) p1 -> next -> prev = node;
      p1 -> next = node;
    }
  }      
}

static inline void remove_domain_list (domain_t *node, domain_t **list) {
  if ((*list) == node)
    (*list) = node -> next;
  
  if (node -> prev)
    node -> prev -> next = node -> next;

  if (node -> next)
    node -> next -> prev = node -> prev;
}

static inline void destroy_domain_list (domain_t **list, 
					void (*process_node) (domain_t *d)) {
  domain_t *p1, *p2;
  p1 = (*list);

  while (p1) {
    p2 = p1;
    p1 = p1 -> next;
    remove_domain_list (p2, list);
    (*process_node) (p2);
    __remove_domain_node (p2);
  }
}

#endif

#endif
