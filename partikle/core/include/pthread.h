/*
 * $FILE: pthread.h
 *
 * POSIX Threads
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

/*
 * XXX: This file is not fully posix-compliant, however the ulibc's
 * pthread.h must be
 */

#ifndef _PTHREAD_H_
#define _PTHREAD_H_

// Exported to ulibc

/* Condition variable-related attributes */
#define PTHREAD_COND_INITIALIZER ((pthread_cond_t){cond_list: 0})

/* Mutex-related attributes */
#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_RECURSIVE 1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT PTHREAD_MUTEX_NORMAL

#define PTHREAD_MUTEX_INITIALIZER \
  ((pthread_mutex_t){magic_number: 0xCBCBCBCB, counter: 0, \
   attr: {prio_protection: 2, prio: 0, type: PTHREAD_MUTEX_DEFAULT, \
   owner_prio: 0}, owner: 0, mutex_list: 0})

#define PTHREAD_PRIO_NONE 0
// PaRTiKle doesn't support PTHREAD_PRIO_INHERIT
//#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2

/* Pthread-related attributes */

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_INHERIT_SCHED 0 
#define PTHREAD_EXPLICIT_SCHED 1

// Cancel state
#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1

// Cancel type
#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

#define PTHREAD_ONCE_INIT {(0)}

// Not exported

#ifdef _KERNEL_

#include <config.h>
#include <linkage.h>
#include <time.h>
#include <types.h>

#define READY_THREAD 0x0 
#define SUSPENDED_THREAD 0x1
#define FINISHED_THREAD 0x2

#define PTHREAD_MUTEX_MAGIC_NUMBER 0xCBCBCBCB
#define PTHREAD_MAGIC_NUMBER 0xA5A55A5A

#define SET_THREAD_STATE(thread, state) { \
  thread -> thread_flags &= (~0x7); \
  thread -> thread_flags |= (state & 0x7); \
}

#define GET_THREAD_STATE(thread) ({ \
  thread -> thread_flags & 0x7; \
})

#define GET_THREAD_DETACH_STATE(thread) ({ \
  thread -> thread_flags & 0x8; \
})

#define SET_THREAD_DETACH_STATE(thread, state) { \
  thread -> thread_flags = (state)? thread -> thread_flags | 0x8: \
    thread -> thread_flags & ~(0x8); \
}

#define GET_THREAD_CANCEL_STATE(thread) ({ \
  thread -> thread_flags & 0x10; \
})

#define SET_THREAD_CANCEL_STATE(thread, state) { \
  thread -> thread_flags = (state)? thread -> thread_flags | 0x10: \
    thread -> thread_flags & ~(0x10); \
}

#define GET_THREAD_CANCEL_TYPE(thread) ({ \
  thread -> thread_flags & 0x20; \
})

#define SET_THREAD_CANCEL_TYPE(thread, state) { \
  thread -> thread_flags = (state)? thread -> thread_flags | 0x20: \
    thread -> thread_flags & ~(0x20); \
}

#define SET_THREAD_POLICY(thread, policy) { \
  thread -> thread_flags &= ~(0x3c0); \
  thread -> thread_flags |= ((policy & 0xf) << 6); \
}

#define GET_THREAD_POLICY(thread) ({ \
  ((thread -> thread_flags & 0x3c0) >> 6); \
})

/*
 * When this bit is set, the thread will always wake up, even with
 * blocked signals
 */

#define SET_THREAD_ACTIVATE_ON_SIGNAL(thread, activate) { \
  thread -> thread_flags = (activate)? thread -> thread_flags |= 0x400: \
    thread -> thread_flags & ~(0x400); \
}

#define GET_THREAD_ACTIVATE_ON_SIGNAL(thread) ({ \
  thread -> thread_flags & 0x400; \
})

extern struct common_key_struct common_key_struct [MAX_PTHREAD_KEYS];

/* Threads */
extern asmlinkage int pthread_create_sys (pthread_t *, 
		                          const pthread_attr_t *,
			                  void *(*)(void *), void *);
extern asmlinkage void pthread_exit_sys (void *);
extern asmlinkage pthread_t pthread_self_sys (void);
extern asmlinkage int pthread_join_sys (pthread_t thread, void **value_ptr);
extern asmlinkage int pthread_detach_sys (pthread_t thread);
extern asmlinkage int pthread_once_sys (pthread_once_t *once_control,
					void (*init_routine)(void));
extern asmlinkage int pthread_cancel_sys (pthread_t thread);
extern asmlinkage int pthread_setcancelstate_sys (int state, int *oldstate);
extern asmlinkage int pthread_setcanceltype_sys (int type, int *oldtype);
extern asmlinkage void pthread_cleanup_pop_sys (int execute);
extern asmlinkage void pthread_cleanup_push_sys (void (*routine)(void*), void *arg);
extern asmlinkage int pthread_getschedparam_sys (pthread_t thread, int *policy,
						 struct sched_param *param);
extern asmlinkage int pthread_setschedparam_sys (pthread_t thread, int policy,
						 const struct sched_param *
						 param);

/* Mutexes */
extern asmlinkage int pthread_mutex_init_sys (pthread_mutex_t *mutex,
					      const pthread_mutexattr_t *attr);
extern asmlinkage int pthread_mutex_destroy_sys (pthread_mutex_t *mutex);
extern asmlinkage int pthread_mutex_lock_sys (pthread_mutex_t *mutex);
extern asmlinkage int pthread_mutex_trylock_sys (pthread_mutex_t *mutex);
extern asmlinkage int pthread_mutex_unlock_sys (pthread_mutex_t *mutex);
extern asmlinkage int pthread_mutex_timedlock_sys (pthread_mutex_t *mutex,
						   const struct 
						   timespec *abs_timeout);

/* Condition variables */
extern asmlinkage int pthread_cond_init_sys (pthread_cond_t *cond,
					     const pthread_condattr_t *attr);
extern asmlinkage int pthread_cond_destroy_sys (pthread_cond_t *cond);
extern asmlinkage int pthread_cond_signal_sys (pthread_cond_t *cond);
extern asmlinkage int pthread_cond_broadcast_sys (pthread_cond_t *cond);
extern asmlinkage int pthread_cond_wait_sys (pthread_cond_t *cond, 
					     pthread_mutex_t *mutex);
extern asmlinkage int pthread_cond_timedwait_sys(pthread_cond_t *cond,
						 pthread_mutex_t *mutex,
						 const struct 
						 timespec *abstime);

/* Specific info */
extern asmlinkage int pthread_key_create_sys (pthread_key_t *key,
					      void (*destructor) (void *));
extern asmlinkage int pthread_setspecific_sys (pthread_key_t key, 
					       const void *value);
extern asmlinkage void *pthread_getspecific_sys (pthread_key_t key);
extern asmlinkage int pthread_key_delete_sys (pthread_key_t key);
extern asmlinkage int pthread_setspecific_for_sys (pthread_key_t key, 
						   pthread_t thread,
						   const void *value);
extern asmlinkage int pthread_getspecific_from_sys (pthread_key_t key, 
						    pthread_t thread,
						    void **value);
#endif
#endif
