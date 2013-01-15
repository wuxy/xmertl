/*
 * $FILE: rtl.h
 *
 * RTLinux _np (Non Portable) Posix
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
 
#include <pthread.h>
#include <sched.h>
#include <time.h>

#ifdef _KERNEL_
#include <arch/arch_types.h>
#else
#include <sys/types.h>
#endif

extern int pthread_delete_np_sys (pthread_t thread);

extern int pthread_setperiod_np_sys (pthread_t thread, struct timespec *value);
extern int pthread_getperiod_np_sys (pthread_t thread, duration_t* period);
