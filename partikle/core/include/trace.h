/*
 * $FILE: trace.h
 *
 * Funtions to use the tracing
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
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

#ifndef _TRACE_H_
#define _TRACE_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <config.h>

enum Tkiwi_event {
	start = 0,	// QUIVI_RELEASE
	stop,		// QUIVI_ENDTASK
	deadline,	// QUIVI_DEADLINE
	execb,		// QUIVI_STARTCPU
	exece,		// QUIVI_STOPCPU
	readyb,		// QUIVI_READY
	readye,		// QUIVI_NOTREADY
	lock,		// QUIVI_MUTEX_LOCK
	unlock,		// QUIVI_MUTEX_UNLOCK
	arrowup,	// QUIVI_EVNTUP
	arrowdown,	// QUIVI_EVNTDOWN
	block,		// QUIVI_SUSPTASK
	vline,		// vertical line
} Tkiwi_event;

typedef struct Traceevent Traceevent;
struct Traceevent {
	int id;				/* Pthread id */
	int etype;			/* Event type */
	duration_t time;	/* time stamp */
	int res;			/* resource */
	char *text;			/* additional text */
};

extern int log_event (struct Traceevent e);

// keep track of resource id's
extern int last_trace_id;	
extern int last_trace_mutex;	
extern int last_trace_condv;
extern int last_trace_sem;

#endif
