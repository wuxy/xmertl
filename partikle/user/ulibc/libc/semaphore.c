/*
 * $FILE: semaphore.c
 *
 * Posix semaphores system calls
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

#include <config.h>
#include <errno.h>
#include <semaphore.h>
#include <sched.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <syscalls.h>

//----------//
// sem_init //
//----------//

_syscall3(int, sem_init, sem_t *, sem, int, pshared, unsigned, value);

//-------------//
// sem_destroy //
//-------------//

_syscall1(int, sem_destroy, sem_t *, sem);

//--------------//
// sem_getvalue //
//--------------//

_syscall2(int, sem_getvalue, sem_t *, sem, int *, sval);

//----------//
// sem_wait //
//----------//

_syscall1(int, sem_wait, sem_t *, sem);

//---------------//
// sem_timedwait //
//---------------//

_syscall2(int, sem_timedwait, sem_t *, sem,  const struct timespec *, abstime);

//-------------//
// sem_trywait //
//-------------//

_syscall1(int, sem_trywait, sem_t *, sem);


//----------//
// sem_post //
//----------//

_syscall1 (int, sem_post, sem_t *, sem);

// following is implemented only to conform posixtestsuite

//----------//
// sem_open //
//----------//

sem_t* sem_open(const char *name, int oflag, ...) {
        sem_t *sem;
        unsigned value = 0;
        int res = 0;
        int mode = 0;
        va_list ap;

        if ((oflag & O_CREAT)) {
                va_start(ap, oflag);
                mode = va_arg(ap, int);
                value = va_arg(ap, unsigned);
                va_end(ap);
        }

        sem = (sem_t *) malloc(sizeof(sem_t));

        if (!sem) {
                SET_ERRNO(ENOSPC);
                return SEM_FAILED;
        }
        
	__syscall3(sem, 0, value, sem_init_nr, res);
	return sem;
}

//-----------//
// sem_close //
//-----------//

int sem_close(sem_t*sem) {
	if (!sem){
		SET_ERRNO(EINVAL);
		return -1;
	}
	free(sem);
	return 0;
}

//------------//
// sem_unlink //
//------------//

int sem_unlink(const char *sem) {
	if (!sem){
		SET_ERRNO(EINVAL);
		return -1;
	}
	return 0;
}
