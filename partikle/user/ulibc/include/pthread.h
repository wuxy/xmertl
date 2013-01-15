/*
 * $FILE: pthread.h
 *
 * Threads
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

#ifndef _ULIBC_PTHREAD_H_
#define _ULIBC_PTHREAD_H_

#include <sys/types.h>
#include <kernel_inc/pthread.h>
//#include <sched.h>
#include <time.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

/*
 * Mutex-related functions
 */

/* 11.3 Mutexes */

extern int pthread_mutexattr_init (pthread_mutexattr_t *attr);
extern int pthread_mutexattr_destroy (pthread_mutexattr_t *attr);
extern int pthread_mutexattr_gettype (const pthread_mutexattr_t *attr,
				      int *type);
extern int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int type);
extern int pthread_mutex_init (pthread_mutex_t *mutex,
			       const pthread_mutexattr_t *attr);
extern int pthread_mutex_destroy (pthread_mutex_t *mutex);
extern int pthread_mutex_lock (pthread_mutex_t *mutex);
extern int pthread_mutex_trylock (pthread_mutex_t *mutex);
extern int pthread_mutex_unlock (pthread_mutex_t *mutex);
extern int pthread_mutex_getprioceiling (const pthread_mutex_t *mutex,
					 int *prioceiling);
extern int pthread_mutex_setprioceiling (pthread_mutex_t *mutex,
					 int prioceiling, int *old_ceiling);

/*extern int pthread_mutex_timedlock (pthread_mutex_t *mutex,
  const struct timespec *abs_timeout); */

/* 11.4 Condition Variables */

extern int pthread_condattr_init (pthread_condattr_t *attr);
extern int pthread_condattr_destroy (pthread_condattr_t *attr);
extern int pthread_cond_init (pthread_cond_t *cond,
			      const pthread_condattr_t *attr);
extern int pthread_cond_destroy (pthread_cond_t *cond);
extern int pthread_cond_signal (pthread_cond_t *cond);
extern int pthread_cond_broadcast (pthread_cond_t *cond);
extern int pthread_cond_wait (pthread_cond_t *cond, pthread_mutex_t *mutex);

extern int pthread_cond_timedwait (pthread_cond_t *cond, 
				   pthread_mutex_t *mutex,
				   const struct timespec *abstime);

/* 13.6 Synchronization Scheduling */

extern int pthread_mutexattr_setprotocol (pthread_mutexattr_t *attr,
					  int protocol);
extern int pthread_mutexattr_getprotocol (const pthread_mutexattr_t *attr,
					  int *protocol);
extern int pthread_mutexattr_setprioceiling (pthread_mutexattr_t *attr,
					     int prioceiling);
extern int pthread_mutexattr_getprioceiling (const pthread_mutexattr_t *attr,
					     int *prioceiling);

/*
 * Thread-related functions
 */

/* 16.- Threads */

extern int pthread_attr_init (pthread_attr_t *attr);
extern int pthread_attr_destroy (pthread_attr_t *attr);
extern int pthread_attr_setstacksize (pthread_attr_t *attr,
				      size_t stacksize);
extern int pthread_attr_getstacksize (const pthread_attr_t *attr,
				      size_t *stacksize);
extern int pthread_attr_setstackaddr (pthread_attr_t *attr,
				      void *stackaddr);
extern int pthread_attr_getstackaddr (pthread_attr_t *attr,
				      void **stackaddr);
extern int pthread_attr_setdetachstate (pthread_attr_t *attr,
					int detachstate);
extern int pthread_attr_getdetachstate (const pthread_attr_t *attr,
					int *detachstate);
extern int pthread_create (pthread_t *thread, const pthread_attr_t *attr,
			   void *(*start_routine)(void *), void *arg);
extern void pthread_exit (void *value_ptr);
extern pthread_t pthread_self (void);
extern int pthread_join (pthread_t thread, void **value_ptr);
extern int pthread_detach (pthread_t thread);
extern int pthread_equal (pthread_t t1, pthread_t t2);
extern int pthread_once (pthread_once_t *once_control,
			 void (*init_routine)(void));
extern int pthread_getschedparam (pthread_t thread, int *policy,
				  struct sched_param *param);
extern int pthread_setschedparam (pthread_t thread, int policy,
				  const struct sched_param *param);

/* 13.5 Thread Scheduling Functions */

extern int pthread_attr_setinheritsched (pthread_attr_t *attr,
					 int inheritsched);
extern int pthread_attr_getinheritsched (pthread_attr_t *attr,
					 int *inheritsched);
extern int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy);
extern int pthread_attr_getschedpolicy (const pthread_attr_t *attr, int *policy);
extern int pthread_attr_setschedparam (pthread_attr_t *attr,
				       const struct sched_param *param);
extern int pthread_attr_getschedparam (const pthread_attr_t *attr,
				       struct sched_param *param);

extern int pthread_setschedparam (pthread_t thread, int policy,
				  const struct sched_param *param);
extern int pthread_getschedparam (pthread_t thread, int *policy,
				  struct sched_param *param);
extern int pthread_setschedprio (pthread_t thread, int prio);

/* 17. Thread-Specific Data */

extern int pthread_key_create (pthread_key_t *key, 
			       void (*destructor) (void *));
extern int pthread_setspecific (pthread_key_t key, const void *value);
extern void *pthread_getspecific (pthread_key_t key);
extern int pthread_key_delete (pthread_key_t key);
/*
extern int pthread_setspecific_for (pthread_key_t key, pthread_t thread,
				    const void *value);
extern int pthread_getspecific_from (pthread_key_t key, pthread_t thread,
				     void **value);

*/
extern void pthread_cleanup_pop(int execute);
extern void pthread_cleanup_push(void (*routine)(void *), void *arg);

extern int pthread_setcancelstate(int state, int *oldstate);
extern int pthread_setcanceltype(int type, int *oldtype);
extern int pthread_cancel(pthread_t thread);

extern void pthread_yield(void);

END_DECLS

#endif
