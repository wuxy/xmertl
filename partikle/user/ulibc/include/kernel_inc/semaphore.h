/*
 * $FILE: semaphore.h
 *
 * POSIX Semaphores
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

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

// Exported to ulibc
#define SEM_VALUE_MAX	((int)((~0u)>>1))
#define SEM_FAILED		((sem_t *) 0)

typedef struct sem_t {
   int value;				// semaphore value
   pthread_list_t *sem_list;	// list of suspended threads

  /* Used to trace the sem, only when tracing is enabled */
#ifdef CONFIG_PORT_DEVTRACE
  int trace_id;
#endif
} sem_t;

// Not exported
#ifdef _KERNEL_

#include <config.h>
#include <linkage.h>
#include <types.h>

extern asmlinkage int sem_init_sys(sem_t * sem, int pshared, unsigned value);
extern asmlinkage int sem_destroy_sys(sem_t * sem);
extern asmlinkage int sem_getvalue_sys(sem_t * sem, int * sval);
extern asmlinkage int sem_wait_sys(sem_t * sem);
extern asmlinkage int sem_timedwait_sys(sem_t * sem, const struct timespec * tp);
extern asmlinkage int sem_trywait_sys(sem_t * sem);
extern asmlinkage int sem_post_sys(sem_t * sem);

// not completely implemented due to lack of fs
//extern asmlinkage	int	sem_close_sys(sem_t *sem);
//extern asmlinkage sem_t* sem_open_sys(const char *, int, ...);
//extern asmlinkage	int	sem_unlink(const char *sem);

#endif

#endif


