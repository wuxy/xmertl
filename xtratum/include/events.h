/*
 * $FILE: events.h
 *
 * XM's event handling structure
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

#ifndef _XM_EVENTS_H_
#define _XM_EVENTS_H_

#ifndef UXMLIB
#include <bitmap.h>

typedef void (*handler_t) (int, struct pt_regs *);

#else
#include <xm/bitmap.h>
#include <xm/arch/irqs.h>

typedef void (*handler_t) (int, void *);
#endif

typedef struct event_handling_struct {  
  volatile bitmap_t intercepted_events;
  volatile bitmap_t masked_events;
  volatile bitmap_t pending_events;
  handler_t event_handler [MAX_EVENTS];
  handler_t trap_handler [MAX_TRAPS];

  volatile unsigned long event_flags;
  unsigned long *heap;
  unsigned long heap_size;
	
  /* XtratuM FIFOi */
  unsigned long fctladdr;
  unsigned long fdataddr[XM_FIFO_COUNT];
	
  /*Share Memory*/
  unsigned long shmctladdr;
  unsigned long shmdataddr[XM_SHM_COUNT];

  /* Serial memory */
  unsigned long serialaddr;

} event_handling_t;

///////////////////// EVENTS ENABLED

#define EVENTS_ENABLED 0x10 // 1 bit (4)

///////////////////// EXEC_USER_IRQS

#define EXEC_UIRQS 0x20

#endif

