/*
 * $FILE: semaphore.h
 *
 * Posix Semaphores
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
 *
  * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
 
#ifndef _ULIBC_SEMAPHORE_H_
#define _ULIBC_SEMAPHORE_H_

#include <sys/types.h>
#include <time.h>
#include <kernel_inc/semaphore.h>

BEGIN_DECLS

/*
 * Semaphore functions
 */

extern int sem_init(sem_t * sem, int pshared, unsigned value);
extern int sem_destroy(sem_t * sem);
extern int sem_getvalue(sem_t * sem, int * sval);
extern int sem_wait(sem_t * sem);
extern int sem_trywait(sem_t * sem);
extern int sem_post(sem_t * sem);

extern int sem_timedwait(sem_t *, const struct timespec *);
extern int sem_close(sem_t *sem);
extern sem_t *sem_open(const char *, int, ...);
extern int sem_unlink(const char *sem);

END_DECLS

#endif
