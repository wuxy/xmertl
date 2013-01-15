/*
 * $FILE: types.h
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

#include <rtl.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <arch/processor.h>
#include <types.h>
#include <stdio.h>
#include <hwtimers.h>

int pthread_delete_np_sys (pthread_t thread) {
	int flags;

	hw_save_flags_and_cli (flags);
	SET_THREAD_CANCEL_STATE (thread, 1); //PTHREAD_CANCEL_ENABLE
	SET_THREAD_CANCEL_TYPE(thread, 1); //PTHREAD_CANCEL_ASYNCHRONOUS
	finish_thread(thread);
	delete_pthread_struct (thread);

	scheduling ();
	hw_restore_flags (flags);

	return 0;
}

int pthread_setperiod_np_sys (pthread_t thread, struct timespec *value) {
	if (!thread || thread -> magic_number != PTHREAD_MAGIC_NUMBER){
		SET_ERRNO(ESRCH);
		return -1;
	}
	if (value -> tv_sec < 0 || value -> tv_nsec < 0){
		SET_ERRNO(EINVAL);
		return -1;
	}
	
	thread -> sched_param.period = (unsigned) value -> tv_sec * NSECS_PER_SEC + value -> tv_nsec;
	return 0;
}

int pthread_getperiod_np_sys (pthread_t thread, duration_t* period) {
	if (!thread || thread -> magic_number != PTHREAD_MAGIC_NUMBER){
		SET_ERRNO(ESRCH);
		return -1;
	}
	if (!period){
		SET_ERRNO(EINVAL);
		return -1;
	}

	*period = thread -> sched_param.period;
	return 0;
}
