/*
 * $FILE: sched.h
 *
 * POSIX threads
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

#ifndef _SCHED_H_
#define _SCHED_H_

// Exported to ulibc

#define MIN_SCHED_PRIORITY 1023
#define MAX_SCHED_PRIORITY 0

#define SCHED_FIFO 0

// Not exported

#ifdef _KERNEL_

#include <linkage.h>
#include <types.h>

extern pthread_t current_thread;

extern int init_sched (void);

extern int create_idle (void);

extern void change_thread_prio (pthread_t thread, int prio);
extern void suspend_thread (pthread_t thread);
extern void activate_thread (pthread_t thread);
extern void finish_thread (pthread_t thread);

extern pthread_t create_pthread_struct (void);
extern void delete_pthread_struct (pthread_t thread);

extern asmlinkage int pthread_yield_sys (void);

extern void scheduling (void);

#endif
#endif
