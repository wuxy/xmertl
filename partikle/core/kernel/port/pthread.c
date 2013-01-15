/*
 * $FILE: pthread.c
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

#include <config.h>
#include <bitmap.h>
#include <linkage.h>
#include <errno.h>
#include <pthread.h>
#include <pthread_list.h>
#include <signal.h>
#include <stddef.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arch/processor.h>
#include <trace.h>

extern int printf(const char *, ...);
struct common_key_struct common_key_struct [MAX_PTHREAD_KEYS];

//--------------------//
// pthread create_sys //
//--------------------//

asmlinkage int pthread_create_sys (pthread_t *thread, 
				   const pthread_attr_t *attr,
				   void *(*start_routine)(void *), 
				   void *args) {
  int flags;

  hw_save_flags_and_cli (flags);


  // Check policy & prio
  if (attr) {
    if (attr->policy != SCHED_FIFO) {
      SET_ERRNO(EINVAL);
      return -1;
    } else {

      if (attr -> sched_param.sched_priority > MIN_SCHED_PRIORITY ||
	  attr -> sched_param.sched_priority < MAX_SCHED_PRIORITY) {
	SET_ERRNO(EINVAL);
	return -1;
      }
    }
  }

  // Creating the pthread structure
  if (!(*thread = create_pthread_struct ())) {
    SET_ERRNO (EAGAIN);
    hw_restore_flags (flags);
    return -1;
  }
  
  /* 
   * Configuring the new thread either with attr (if not NULL)
   * or with the default values
   */
  if (attr) {
    (*thread) -> sched_param = attr -> sched_param;    
    (*thread) -> stack_info.stack_size = attr -> stack_size;
    (*thread) -> stack_info.stack_bottom = attr -> stack_addr;
    SET_THREAD_DETACH_STATE((*thread), attr -> detachstate);
    SET_THREAD_POLICY ((*thread), attr -> policy);
  } else {
    (*thread) -> sched_param.sched_priority = MIN_SCHED_PRIORITY;
    (*thread) -> stack_info.stack_size = STACK_SIZE;
    (*thread) -> stack_info.stack_bottom = 0;
    SET_THREAD_DETACH_STATE((*thread), 0);
    SET_THREAD_POLICY ((*thread), SCHED_FIFO);
  }

  if (!((*thread) -> stack_info.stack_bottom)) {
    // Creating the thread stack
    if (alloc_stack (&(*thread) -> stack_info) < 0) {
      SET_ERRNO (EAGAIN);
      hw_restore_flags (flags);
      return -1;
    }
  }

  // This is arhictecture dependent
  (*thread) -> stack = setup_stack ((*thread)->stack_info.stack_bottom +
				    (*thread)->stack_info.stack_size 
				    / sizeof (int),
                                    start_routine, args);

  activate_thread (*thread);

  // no error at all
  hw_restore_flags (flags);

  // Calling the scheduler
  scheduling ();
  return 0;
}

//------------------//
// pthread_self_sys //
//------------------//

asmlinkage pthread_t pthread_self_sys (void) {
  return current_thread;
}

//------------------//
// pthread_join_sys //
//------------------//

asmlinkage int pthread_join_sys (pthread_t thread, void **value_ptr) {
  int flags, c_state;

  if (!thread || thread -> magic_number != PTHREAD_MAGIC_NUMBER) {
    SET_ERRNO (ESRCH);
    return -1;
  }

  if (GET_THREAD_DETACH_STATE(thread)) {
    SET_ERRNO (EINVAL);
    return -1;
  }

  hw_save_flags_and_cli (flags);
  c_state = GET_THREAD_STATE (thread);

  while (c_state != FINISHED_THREAD) {
    thread -> joining_thread = current_thread;
    suspend_thread (current_thread);

    scheduling ();
    
    c_state = GET_THREAD_STATE (thread);

    if (GET_THREAD_DETACH_STATE(thread)) {
      hw_restore_flags (flags);
      SET_ERRNO (EINVAL);
      return -1;
    }
  }

  if (value_ptr)
    *value_ptr = thread -> exit_value;

  delete_pthread_struct (thread);
  hw_restore_flags (flags);

  return 0;
}

//--------------------//
// pthread_detach_sys //
//--------------------//

asmlinkage int pthread_detach_sys (pthread_t thread) {
  int flags;

  if (!thread || thread -> magic_number != PTHREAD_MAGIC_NUMBER) {
    SET_ERRNO (ESRCH);
    return -1;
  }

  if (GET_THREAD_DETACH_STATE(thread)) {
    SET_ERRNO (EINVAL);
    return -1;
  }

  hw_save_flags_and_cli (flags);
  SET_THREAD_DETACH_STATE(thread, 1);
  
  activate_thread (current_thread -> joining_thread);
  current_thread -> joining_thread = 0;
  hw_restore_flags (flags);
  scheduling ();

  return 0;
}

//--------------------//
// pthread_cancel_sys //
//--------------------//

asmlinkage int pthread_cancel_sys (pthread_t thread) {
  if (!thread || thread -> magic_number != PTHREAD_MAGIC_NUMBER) {
    SET_ERRNO (ESRCH);
    return -1;
  }

  PTHREAD_CANCEL (thread);
  
  return 0;
}

//----------------------------//
// pthread_setcancelstate_sys //
//----------------------------//

asmlinkage int pthread_setcancelstate_sys (int state, int *oldstate) {

  if (oldstate)
    *oldstate = (GET_THREAD_CANCEL_STATE(current_thread))? 
      PTHREAD_CANCEL_DISABLE:PTHREAD_CANCEL_ENABLE;
  
  if (state == PTHREAD_CANCEL_DISABLE) {
    SET_THREAD_CANCEL_STATE(current_thread, 1);
  } else {
    SET_THREAD_CANCEL_STATE(current_thread, 0);
  }

 return 0;
}

//---------------------------//
// pthread_setcanceltype_sys //
//---------------------------//

asmlinkage int pthread_setcanceltype_sys (int type, int *oldtype) {
  if (oldtype)
    *oldtype = (GET_THREAD_CANCEL_TYPE(current_thread))? 
      PTHREAD_CANCEL_ASYNCHRONOUS:PTHREAD_CANCEL_DEFERRED;
  
  if (type == PTHREAD_CANCEL_ASYNCHRONOUS) {
    SET_THREAD_CANCEL_TYPE(current_thread, 1);
  } else {
    SET_THREAD_CANCEL_TYPE(current_thread, 0);
  }

  return 0;
}

//-------------------------//
// pthread_cleanup_pop_sys //
//-------------------------//

asmlinkage void pthread_cleanup_pop_sys (int execute) {
  struct cleanup_routine cr;
  if (current_thread -> top_cr_table > 0) {
    cr = current_thread -> cleanup_routine_table 
      [--current_thread -> top_cr_table];
    
    if (execute)
      CALL_CANCEL_HANDLER (cr.routine, cr.arg);
  }
}

//--------------------------//
// pthread_cleanup_push_sys //
//--------------------------//

asmlinkage void pthread_cleanup_push_sys (void (*routine)(void *), void *arg) {
  if (current_thread -> top_cr_table < MAX_CLEANUP_HANDLERS) {
    current_thread -> cleanup_routine_table 
      [current_thread -> top_cr_table++] = 
      (struct cleanup_routine){routine:routine, arg:arg};
  }
}

//------------------//
// pthread_once_sys //
//------------------//

asmlinkage int pthread_once_sys (pthread_once_t *once_control,
				 void (*init_routine)(void)) {
  int init = 0;
  if (!once_control || !init_routine) {
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  // once_control -> init is atomic, it shouldn't happen anything
  init = ++once_control -> init;

  if (init)
    return 0;

  (*init_routine) ();
  
  return 0;
}

//------------------//
// pthread_exit_sys //
//------------------//
extern pthread_t mainapp_thread;
asmlinkage void pthread_exit_sys (void *value_ptr) {
  struct cleanup_routine cr;

  // Disabling all the thread's alarm clock
  free_ktimer (current_thread -> sleep_ktimer);
  finish_thread (current_thread);
  current_thread -> exit_value = value_ptr;

  while (current_thread -> top_cr_table > 0) {
    cr = current_thread -> cleanup_routine_table 
      [--current_thread -> top_cr_table];
    CALL_CANCEL_HANDLER (cr.routine, cr.arg);
  }

  if (!GET_THREAD_DETACH_STATE(current_thread) && 
      current_thread -> joining_thread)
    activate_thread (current_thread -> joining_thread);

  /*
    TODO: Check that all work properly
   */

  if (mainapp_thread == current_thread) {
    exit_sys ((unsigned long)value_ptr);
  }

  scheduling ();
}

//---------------------------//
// pthread_getschedparam_sys //
//---------------------------//

asmlinkage int pthread_getschedparam_sys (pthread_t thread, int *policy,
					  struct sched_param *param) {
  if (!thread || thread -> magic_number != PTHREAD_MAGIC_NUMBER) {
    SET_ERRNO (ESRCH);
    return -1;
  }

  if (policy)
    *policy = GET_THREAD_POLICY(thread);
  
  if (param)
    *param = thread -> sched_param;

  return 0;
}

//---------------------------//
// pthread_setschedparam_sys //
//---------------------------//

asmlinkage int pthread_setschedparam_sys (pthread_t thread, int policy,
					  const struct sched_param *param) {

  int flags;
  
  if (!thread || thread -> magic_number != PTHREAD_MAGIC_NUMBER) {
    SET_ERRNO (ESRCH);
    return -1;
  }
  
  hw_save_flags_and_cli (flags);
  SET_THREAD_POLICY(thread, policy);
  if (param)
    thread -> sched_param = *param;
  else {
    hw_restore_flags (flags);
    SET_ERRNO (EINVAL);
    return -1;
  }

  change_thread_prio (thread, thread -> sched_param.sched_priority);
  hw_restore_flags (flags);

  scheduling ();

  return 0;
}

//------------------------//
// pthread_mutex_init_sys //
//------------------------//

asmlinkage int pthread_mutex_init_sys (pthread_mutex_t *mutex,
				       const pthread_mutexattr_t *attr) {
  if (mutex) {
    *mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    if (attr)
      mutex -> attr = *attr;
  } else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  return 0;
}

//---------------------------//
// pthread_mutex_destroy_sys //
//---------------------------//

asmlinkage int pthread_mutex_destroy_sys (pthread_mutex_t *mutex) {
  return 0;
}

//------------------------//
// pthread_mutex_lock_sys //
//------------------------//

asmlinkage int pthread_mutex_lock_sys (pthread_mutex_t *mutex) {
  int flags;

  hw_save_flags_and_cli (flags);
  
  while (mutex -> owner) {
    if (mutex -> owner == current_thread) {
      switch (mutex -> attr.type) {
      case PTHREAD_MUTEX_ERRORCHECK:
	hw_restore_flags (flags);
	SET_ERRNO (EDEADLK);
	return -1;
	break;
      case PTHREAD_MUTEX_RECURSIVE:
	mutex -> counter ++;
	goto out_mutex_lock;
	break;
      }
    }
    /* 
     * Some thread already has locked the mutex so suspending the
     * caller thread
     */

    suspend_thread (current_thread);

    insert_pthread_list (&mutex -> mutex_list, &current_thread -> mutex_list);
    
    scheduling ();
  }

  /*
   * Locking the mutex
   */
  mutex -> owner = current_thread;
  mutex -> counter = 1;
  if (mutex -> attr.prio_protection == PTHREAD_PRIO_PROTECT) {
    mutex -> attr.owner_prio = current_thread -> sched_param.sched_priority;
    if (current_thread -> sched_param.sched_priority >  mutex -> attr.prio) {
      change_thread_prio (current_thread, mutex -> attr.prio);
      scheduling ();
    }
  }
 out_mutex_lock:
  hw_restore_flags (flags);
  return 0;
}

//---------------------------//
// pthread_mutex_trylock_sys //
//---------------------------//

asmlinkage int pthread_mutex_trylock_sys (pthread_mutex_t *mutex) {
  int flags;

  hw_save_flags_and_cli (flags);
  
  if (mutex -> owner) {
    if (mutex -> owner == current_thread) {
      if (mutex -> attr.type == PTHREAD_MUTEX_RECURSIVE) {
	mutex -> counter ++;
	goto out_mutex_try_lock;
      }
    }
    hw_restore_flags (flags);
    SET_ERRNO (EBUSY);
    return -1;
    
  }

  /*
   * Locking the mutex
   */
  mutex -> owner = current_thread;
  if (mutex -> attr.prio_protection == PTHREAD_PRIO_PROTECT) {
    mutex -> attr.owner_prio = current_thread -> sched_param.sched_priority;
    if (current_thread -> sched_param.sched_priority >  mutex -> attr.prio) {
      change_thread_prio (current_thread, mutex -> attr.prio);
      scheduling ();
    }
  }
    
 out_mutex_try_lock:
  hw_restore_flags (flags);
  return 0;
}

//--------------------------//
// pthread_mutex_unlock_sys //
//--------------------------//

asmlinkage int pthread_mutex_unlock_sys (pthread_mutex_t *mutex) {
  int flags;
  pthread_t thread;

  hw_save_flags_and_cli (flags);
  if (mutex -> owner) {
    if (mutex -> owner != current_thread) {
      hw_restore_flags (flags);
      SET_ERRNO (EPERM);
      return -1;
    }
    
    if (mutex -> attr.type == PTHREAD_MUTEX_RECURSIVE) {
      mutex -> counter --;
      if (mutex -> counter > 0) goto out_mutex_unlock;
    }

    while((thread = remove_head_pthread_list (&mutex -> mutex_list))){
      activate_thread (thread);
    }
    
    mutex -> mutex_list = NULL;
    mutex -> owner = NULL;
    mutex -> counter = 0;
    if ((mutex -> attr.prio_protection == PTHREAD_PRIO_PROTECT) &&
        (mutex -> attr.owner_prio != current_thread -> sched_param.sched_priority)){
         change_thread_prio (current_thread, mutex -> attr.owner_prio);
         scheduling ();
    }
  } else {
    if (mutex -> attr.type == PTHREAD_MUTEX_ERRORCHECK) {
      hw_restore_flags (flags);
      SET_ERRNO (EPERM);
      return -1;
    }
  }

 out_mutex_unlock:
  hw_restore_flags (flags);
  return 0;
}

//-----------------------------//
// pthread_mutex_timedlock_sys //
//-----------------------------//

/*
 * We need signals to implement this function
 */

asmlinkage int pthread_mutex_timedlock_sys (pthread_mutex_t *mutex,
					    const 
					    struct timespec *abs_timeout) {
  return 0;
}

//-----------------------//
// pthread_cond_init_sys //
//-----------------------//

asmlinkage int pthread_cond_init_sys (pthread_cond_t *cond,
				      const pthread_condattr_t *attr) {
  if (cond) {
    *cond = PTHREAD_COND_INITIALIZER;
  } else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  return 0;
}

//--------------------------//
// pthread_cond_destroy_sys //
//--------------------------//

asmlinkage int pthread_cond_destroy_sys (pthread_cond_t *cond) {
  return 0;
}

//-------------------------//
// pthread_cond_signal_sys //
//-------------------------//

asmlinkage int pthread_cond_signal_sys (pthread_cond_t *cond) {
  int flags;
  pthread_t thread;

  if (!cond) {
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  hw_save_flags_and_cli (flags);

  if ((thread = remove_head_pthread_list (&cond -> cond_list)))
    activate_thread (thread);
  scheduling ();
  hw_restore_flags (flags);

  return 0;
}

//----------------------------//
// pthread_cond_broadcast_sys //
//----------------------------//

asmlinkage int pthread_cond_broadcast_sys (pthread_cond_t *cond) {
  int flags;
  pthread_t thread;

  if (!cond) {
    SET_ERRNO (EINVAL);
    return -1;
  }

  hw_save_flags_and_cli (flags);
  
  while ((thread = remove_head_pthread_list (&cond -> cond_list))){
    activate_thread (thread); 
  }
  scheduling ();
  hw_restore_flags (flags);

  return 0;
}

//-----------------------//
// pthread_cond_wait_sys //
//-----------------------//

asmlinkage int pthread_cond_wait_sys (pthread_cond_t *cond, 
				      pthread_mutex_t *mutex) {
  int flags;
  
  if (!cond || !mutex) {
    SET_ERRNO (EINVAL);
    return -1;
  }

  if (mutex -> owner != current_thread) {
    SET_ERRNO (EPERM);
    return -1;
  }

  hw_save_flags_and_cli (flags);
  pthread_mutex_unlock_sys (mutex);
  suspend_thread (current_thread);
  insert_pthread_list (&cond -> cond_list, &current_thread -> cond_list);
  scheduling ();

  pthread_mutex_lock_sys (mutex);
  hw_restore_flags (flags);
  return 0;
}

//----------------------------//
// pthread_cond_timedwait_sys //
//----------------------------//

asmlinkage int pthread_cond_timedwait_sys (pthread_cond_t *cond,
					   pthread_mutex_t *mutex,
					   const struct timespec *abstime) {
  duration_t d, r;
  int time_error = 0, flags;
  
  if (!cond || !mutex || !abstime){
    SET_ERRNO (EINVAL);
    return -1;
  }

  d = abstime -> tv_sec * NSECS_PER_SEC + abstime -> tv_nsec  
    - realtime_clock->gettime_nsec ();

  if (mutex -> owner != current_thread) {
    SET_ERRNO (EPERM);
    return -1;
  }

  hw_save_flags_and_cli (flags);
  pthread_mutex_unlock_sys (mutex);
  
  insert_pthread_list (&cond -> cond_list, &current_thread -> cond_list);
  r = sleep_for (d);
  remove_pthread_list (&cond -> cond_list, &current_thread -> cond_list);

  pthread_mutex_lock_sys (mutex);
  if (r < monotonic_clock->gettime_nsec ()) 
    time_error = 1;

  hw_restore_flags (flags);

  if (time_error) {
    SET_ERRNO (ETIMEDOUT);
    return -1;
  }

  return 0;
}

//------------------------//
// pthread_key_create_sys //
//------------------------//

asmlinkage int pthread_key_create_sys (pthread_key_t *key,
				       void (*destructor) (void *)) {
  int k;
  
  for (k = 0; k < MAX_PTHREAD_KEYS; k++)
    if (! common_key_struct [k].in_use) break;

  if (k >= MAX_PTHREAD_KEYS) {
    SET_ERRNO (EAGAIN);
    return -1;
  }

  common_key_struct [k].in_use = 1;
  common_key_struct [k].destructor = destructor;

  if (key) 
    *key = k;

  return 0;
}

//-------------------------//
// pthread_setspecific_sys //
//-------------------------//

asmlinkage int pthread_setspecific_sys (pthread_key_t key, 
					const void *value) {
  if (key < 0 || key > MAX_PTHREAD_KEYS - 1 ||
      ! common_key_struct [key].in_use) {
    SET_ERRNO (EINVAL);
    return -1;
  }

  current_thread -> tsd [key].i = (void *) value;
  return 0;
}

//-------------------------//
// pthread_getspecific_sys //
//-------------------------//

asmlinkage void *pthread_getspecific_sys (pthread_key_t key) {
  if (key < 0 || key > MAX_PTHREAD_KEYS - 1 ||
      !common_key_struct [key].in_use) {
    SET_ERRNO (EINVAL);
    return NULL;
  }

  return current_thread -> tsd [key].i;
}

//------------------------//
// pthread_key_delete_sys //
//------------------------//

asmlinkage int pthread_key_delete_sys (pthread_key_t key) {

  if (key < 0 || key > MAX_PTHREAD_KEYS - 1 ||
      !common_key_struct [key].in_use) {
    SET_ERRNO (EINVAL);
    return -1;
  }

  if (common_key_struct [key].destructor)
    common_key_struct [key].destructor (current_thread -> tsd [key].i);
  
  common_key_struct [key].destructor = NULL;
  common_key_struct [key].in_use = 0;

  return 0;
}

//-----------------------------//
// pthread_setspecific_for_sys //
//-----------------------------//
/*
asmlinkage int pthread_setspecific_for_sys (pthread_key_t key, 
					    pthread_t thread, 
					    const void *value) {
  if (key < 0 || key > MAX_PTHREADS_KEY - 1) {
    SET_ERRNO (EINVAL);
    return -1;
  }
  if (!thread) threads = pthread_self_sys ();

  return 0;
}
*/
//------------------------------//
// pthread_getspecific_from_sys //
//------------------------------//
/*
asmlinkage int pthread_getspecific_from_sys (pthread_key_t key, 
					     pthread_t thread,
					     void **value) {
  if (key < 0 || key > MAX_PTHREADS_KEY - 1) {
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  if (!thread) threads = pthread_self_sys ();

  

  return 0;
}
*/
