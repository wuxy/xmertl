/*
 * $FILE: unistd.h
 *
 * Standard symbolic constants and types
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
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

#ifndef _ULIBC_UNISTD_H_
#define _ULIBC_UNISTD_H_

#include <sys/types.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

/* Conformance with the Minimal Realtime System Profile */
//#define _POSIX_AEP_REALTIME_MINIMAL  1

/* Presence of optional features */
//#define _POSIX_AEP_REALTIME_LANG_C89  1

/* Job control is NOT supported.  */
//#define _POSIX_JOB_CONTROL // NOT suported in PSE51

/* Processes have a saved set-user-ID and a saved set-group-ID.  */
//#define _POSIX_SAVED_IDS // NOT suported in PSE51

/* Priority scheduling is supported.  */
//#define _POSIX_PRIORITY_SCHEDULING      1

/* Synchronizing file data is supported.  */
//#define _POSIX_SYNCHRONIZED_IO  1

/* The fsync function is present.  */
//#define _POSIX_FSYNC    1 Not yet

/* Mapping of files to memory is supported.  */
//#define _POSIX_MAPPED_FILES // NOT suported in PSE51

/* Locking of all memory is supported.  */
//#define _POSIX_MEMLOCK  1 Not yet

/* Locking of ranges of memory is supported.  */
//#define _POSIX_MEMLOCK_RANGE    1 Not yet

/* Setting of memory protections is supported.  */
//#define _POSIX_MEMORY_PROTECTION // NOT suported in PSE51

/* Implementation supports `poll' function.  */
//#define _POSIX_POLL     1 ??

/* Implementation supports `select' and `pselect' functions.  */
//#define _POSIX_SELECT   1 ??

/* Only root can change owner of file.  */
//#define _POSIX_CHOWN_RESTRICTED // NOT suported in PSE51

/* `c_cc' member of 'struct termios' structure can be disabled by
   using the value _POSIX_VDISABLE.  */
//#undef _POSIX_VDISABLE // NOT suported in PSE51

/* Filenames are not silently truncated.  */
//#define _POSIX_NO_TRUNC 1

/* Tell we have POSIX threads.  */
//#define _POSIX_THREADS  1

/* We have the reentrant functions described in POSIX.  */
//#define _POSIX_REENTRANT_FUNCTIONS      1 ??
//#define _POSIX_THREAD_SAFE_FUNCTIONS    1 Not yet

/* We provide priority scheduling for threads.  */
//#define _POSIX_THREAD_PRIORITY_SCHEDULING       1

/* We provide priority protection protocol for mutexes.  */
//#define _POSIX_THREAD_PRIO_PROTECT       1

/* We provide priority inheritance protocol for mutexes.  */
//#define _POSIX_THREAD_PRIO_INHERIT       1

/* We support user-defined stack sizes.  Not yet */
//#undef _POSIX_THREAD_ATTR_STACKSIZE

/* We support user-defined stacks.  Not yet  */
//#define _POSIX_THREAD_ATTR_STACKADDR

/* Semaphores are not supported.  */
//#define _POSIX_SEMAPHORES

/* Real-time signals are supported.  */
//#define _POSIX_REALTIME_SIGNALS 1 Not yet

/* We support asynchronous I/O.  */
//#define _POSIX_ASYNCHRONOUS_IO // NOT suported in PSE51

/* We support POSIX timers.  */
//#define _POSIX_TIMERS  1

/* Message queues are not supported.  */
//#define _POSIX_MESSAGE_PASSING

/* We support POSIX timers.  */
//#define _POSIX_TIMERS  1

/* Message queues are not supported.  */
//#define _POSIX_MESSAGE_PASSING

/* POSIX Version  */
#define _POSIX_VERSION  199506L

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

extern int close (int);
extern ssize_t read (int, void *, size_t);
extern ssize_t write(int, const void *, size_t);
extern int usleep(useconds_t usec);

/* All the threads live a single process whose pid is 2 and was
 * magically created by process 1 */
static inline pid_t getpid(void)  {return 2;};
static inline pid_t getppid(void) {return 1;};

END_DECLS

#endif
