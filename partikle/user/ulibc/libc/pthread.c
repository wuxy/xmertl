/*
 * $FILE: pthread.c
 *
 * Posix threads system calls
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

#include <config.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <syscalls.h>

//----------------//
// pthread_create //
//----------------//

int pthread_create(pthread_t *thread,
		   const pthread_attr_t *attr,
		   void *(*start_routine)(void *), void *arg) {
  long __res;
  __syscall4 (thread, attr, start_routine, arg, pthread_create_nr, __res);
  __syscall_return (int,__res);
}

//--------------//
// pthread_exit //
//--------------//

void pthread_exit (void *value_ptr) {
  long __res;
  __syscall1 (value_ptr, pthread_exit_nr, __res);
}

//--------------//
// pthread_join //
//--------------//

_syscall2 (int, pthread_join, pthread_t, thread, void **, value_ptr);

//----------------//
// pthread_detach //
//----------------//

_syscall1 (int, pthread_detach, pthread_t, thread);

//--------------//
// pthread_once //
//--------------//

int pthread_once (pthread_once_t *once_control,
		  void (*init_routine)(void)) {
  long __res;

  __syscall2 (once_control, init_routine, pthread_once_nr, __res);
  __syscall_return (int,__res);
}

//--------------//
// pthread_self //
//--------------//

pthread_t pthread_self (void) {
  long __res;
  __syscall0 (pthread_self_nr, __res);
  return (pthread_t) __res;
}

//-----------------------//
// pthread_getschedparam //
//-----------------------//

_syscall3 (int, pthread_getschedparam, pthread_t, thread, int *, policy,
	   struct sched_param *, param);

//-----------------------//
// pthread_setschedparam //
//-----------------------//

_syscall3 (int, pthread_setschedparam, pthread_t, thread, int, policy,
	   const struct sched_param *, param);

//-------------------//
// pthread_attr_init //
//-------------------//

int pthread_attr_init (pthread_attr_t *attr) {
  if (attr) {
    *attr = (pthread_attr_t) {      
      stack_size: STACK_SIZE,
      stack_addr: 0,
      sched_param: {sched_priority: sched_get_priority_min (SCHED_FIFO)},
      detachstate: 0};
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//----------------------//
// pthread_attr_destroy //
//----------------------//

int pthread_attr_destroy (pthread_attr_t *attr) {
  return 0;
}

//---------------------------//
// pthread_attr_setstacksize //
//---------------------------//

int pthread_attr_setstacksize (pthread_attr_t *attr, size_t stacksize) {
  if (attr) {
    attr -> stack_size = stacksize;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//---------------------------//
// pthread_attr_getstacksize //
//---------------------------//

int pthread_attr_getstacksize (const pthread_attr_t *attr, size_t *stacksize) {
  if (attr && stacksize) {
    *stacksize = attr  -> stack_size;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//---------------------------//
// pthread_attr_setstackaddr //
//---------------------------//

int pthread_attr_setstackaddr (pthread_attr_t *attr, void *stackaddr) {
  if (attr) {
    attr -> stack_addr = stackaddr;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//---------------------------//
// pthread_attr_getstackaddr //
//---------------------------//

int pthread_attr_getstackaddr (pthread_attr_t *attr, void **stackaddr) {
  if (attr && stackaddr) {
    *stackaddr = attr -> stack_addr;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//-----------------------------//
// pthread_attr_setdetachstate //
//-----------------------------//

int pthread_attr_setdetachstate (pthread_attr_t *attr, int detachstate) {
  if (attr) {
    attr -> detachstate = (detachstate == PTHREAD_CREATE_DETACHED)? 1:0;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//-----------------------------//
// pthread_attr_getdetachstate //
//-----------------------------//

int pthread_attr_getdetachstate (const pthread_attr_t *attr, 
				 int *detachstate) {
  if (attr && detachstate) {
    *detachstate = (attr -> detachstate)?
      PTHREAD_CREATE_DETACHED:PTHREAD_CREATE_JOINABLE;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
    
  return 0;
}

//------------------------------//
// pthread_attr_setinheritsched //
//------------------------------//

int pthread_attr_setinheritsched (pthread_attr_t *attr,
				  int inheritsched) {
  if (!attr) {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//------------------------------//
// pthread_attr_getinheritsched //
//------------------------------//

int pthread_attr_getinheritsched (pthread_attr_t *attr,
				  int *inheritsched) {
  if (!attr || !inheritsched) {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//-----------------------------//
// pthread_attr_setschedpolicy //
//-----------------------------//

int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy) {
  if (attr) {
    attr -> policy = policy;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//-----------------------------//
// pthread_attr_getschedpolicy //
//-----------------------------//

int pthread_attr_getschedpolicy (const pthread_attr_t *attr, int *policy) {
  if (attr && policy) {
    *policy = attr -> policy;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//----------------------------//
// pthread_attr_setschedparam //
//----------------------------//

int pthread_attr_setschedparam (pthread_attr_t *attr,
				const struct sched_param *param) {
  if (attr && param) {
    attr -> sched_param = *param;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//----------------------------//
// pthread_attr_getschedparam //
//----------------------------//

int pthread_attr_getschedparam (const pthread_attr_t *attr,
				struct sched_param *param) {
  if (attr && param) {
    *param = attr -> sched_param;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//------------------------//
// pthread_setcancelstate //
//------------------------//

_syscall2 (int, pthread_setcancelstate, int, state, int *, oldstate);

//-----------------------//
// pthread_setcanceltype //
//-----------------------//

_syscall2 (int, pthread_setcanceltype, int, type, int *, oldtype);

//----------------//
// pthread_cancel //
//----------------//

_syscall1 (int, pthread_cancel, pthread_t, thread);

//---------------------//
// pthread_cleanup_pop //
//---------------------//

void pthread_cleanup_pop (int execute) {
  long __res;
  __syscall1 (execute, pthread_cleanup_pop_nr, __res);
}

//----------------------//
// pthread_cleanup_push //
//----------------------//

void pthread_cleanup_push (void (*routine)(void *), void *arg) {
  long __res;
  __syscall2 (routine, arg, pthread_cleanup_push_nr, __res);
}

//---------------//
// pthread_equal //
//---------------//

int pthread_equal (pthread_t p1, pthread_t p2) {
  return (p1 == p2);
}


//--------------------//
// pthread_mutex_init //
//--------------------//

_syscall2 (int, pthread_mutex_init, pthread_mutex_t *, mutex,
	   const pthread_mutexattr_t *, attr);

//-----------------------//
// pthread_mutex_destroy //
//-----------------------//

_syscall1 (int, pthread_mutex_destroy, pthread_mutex_t *, mutex);

//--------------------//
// pthread_mutex_lock //
//--------------------//

_syscall1 (int, pthread_mutex_lock, pthread_mutex_t *, mutex);

//-------------------------//
// pthread_mutex_timedlock //
//-------------------------//

_syscall2 (int, pthread_mutex_timedlock, pthread_mutex_t *, mutex, 
	   const struct timespec *, abs_timeout);

//-----------------------//
// pthread_mutex_trylock //
//-----------------------//

_syscall1 (int, pthread_mutex_trylock, pthread_mutex_t *, mutex);

//----------------------//
// pthread_mutex_unlock //
//----------------------//

_syscall1 (int, pthread_mutex_unlock, pthread_mutex_t *, mutex);

//------------------------------//
// pthread_mutex_getprioceiling //
//------------------------------//

int pthread_mutex_getprioceiling (const pthread_mutex_t *mutex,
				  int *prioceiling) {
  if (!mutex || mutex -> magic_number != 0xCBCBCBCB) {
    _set_errno (EINVAL);
    return -1;
  }
  
  if (prioceiling) 
    *prioceiling = mutex -> attr.prio;

  return 0;
}

//------------------------------//
// pthread_mutex_setprioceiling //
//------------------------------//

int pthread_mutex_setprioceiling (pthread_mutex_t *mutex,
				  int prioceiling, int *old_ceiling) {
  if (!mutex || mutex -> magic_number != 0xCBCBCBCB) {
    _set_errno (EINVAL);
    return -1;
  }

  /*
   * TODO: Check if prioceiling is out of range
   */

  pthread_mutex_lock (mutex);  
  if (old_ceiling) *old_ceiling = mutex -> attr.prio;
  mutex -> attr.prio = prioceiling;
  pthread_mutex_unlock (mutex);
  return 0;
}

//------------------------//
// pthread_mutexattr_init //
//------------------------//

/* Our default values are: PTHREAD_PRIO_PROTECT with the scheduler's 
   maximum priority */
int pthread_mutexattr_init (pthread_mutexattr_t *attr) {
  if (attr) {
    *attr = (pthread_mutexattr_t) {
      prio_protection: PTHREAD_PRIO_PROTECT,
      type: PTHREAD_MUTEX_DEFAULT,
      prio: sched_get_priority_max (SCHED_FIFO),
      owner_prio: 0};
  } else {
    _set_errno (EINVAL);
    return -1;
  }

  return 0;
}

//---------------------------//
// pthread_mutexattr_destroy //
//---------------------------//

int pthread_mutexattr_destroy (pthread_mutexattr_t *attr) {
  
  return 0;
}

//-------------------------------//
// pthread_mutexattr_setprotocol //
//-------------------------------//

int pthread_mutexattr_setprotocol (pthread_mutexattr_t *attr,
				   int protocol) {
  if (attr) {
    attr -> prio_protection = protocol;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//-------------------------------//
// pthread_mutexattr_getprotocol //
//-------------------------------//

int pthread_mutexattr_getprotocol (const pthread_mutexattr_t *attr,
				   int *protocol) {
  if (attr && protocol) {
    *protocol = attr -> prio_protection;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//----------------------------------//
// pthread_mutexattr_setprioceiling //
//----------------------------------//

int pthread_mutexattr_setprioceiling (pthread_mutexattr_t *attr,
				      int prioceiling) {
  if (attr) {
    attr -> prio = prioceiling;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//----------------------------------//
// pthread_mutexattr_getprioceiling //
//----------------------------------//

int pthread_mutexattr_getprioceiling (const pthread_mutexattr_t *attr,
				      int *prioceiling) {
  if (attr && prioceiling) {
    *prioceiling = attr -> prio;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//---------------------------//
// pthread_mutexattr_gettype //
//---------------------------//

int pthread_mutexattr_gettype (const pthread_mutexattr_t *attr,
			       int *type) {
  if (attr && type) {
    *type = attr -> type;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
   return 0;
}

//---------------------------//
// pthread_mutexattr_settype //
//---------------------------//

int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int type) {
  if (attr) {
    attr -> type = type;
  } else {
    _set_errno (EINVAL);
    return -1;
  }
  return 0;
}

//-------------------//
// pthread_cond_init //
//-------------------//

_syscall2 (int, pthread_cond_init, pthread_cond_t *, cond, 
	   const pthread_condattr_t *, attr);

//----------------------//
// pthread_cond_destroy //
//----------------------//

_syscall1 (int, pthread_cond_destroy, pthread_cond_t *, cond);

//---------------------//
// pthread_cond_signal //
//---------------------//

_syscall1 (int, pthread_cond_signal, pthread_cond_t *, cond);

//------------------------//
// pthread_cond_broadcast //
//------------------------//

_syscall1 (int, pthread_cond_broadcast, pthread_cond_t *, cond);

//-------------------//
// pthread_cond_wait //
//-------------------//

_syscall2 (int, pthread_cond_wait, pthread_cond_t *, cond, 
	   pthread_mutex_t *, mutex);

//------------------------//
// pthread_cond_timedwait //
//------------------------//

_syscall3 (int, pthread_cond_timedwait, pthread_cond_t *, cond,
	   pthread_mutex_t *, mutex,
	   const struct timespec *, abstime);

//--------------------------//
// pthread_condattr_destroy //
//--------------------------//

int pthread_condattr_destroy(pthread_condattr_t *attr) {
  return 0;
}

//-----------------------//
// pthread_condattr_init //
//-----------------------//

int pthread_condattr_init(pthread_condattr_t *attr) {
  return 0;
}

//--------------------//
// pthread_key_create //
//--------------------//
int pthread_key_create (pthread_key_t *key,
			void (*destructor) (void *)) {
  long __res;
  __syscall2 (key, destructor, pthread_key_create_nr, __res);
  __syscall_return (int,__res);
}

//---------------------//
// pthread_setspecific //
//---------------------//

_syscall2 (int, pthread_setspecific, pthread_key_t, key, const void *, value);

//---------------------//
// pthread_getspecific //
//---------------------//

_syscall1 (void *, pthread_getspecific, pthread_key_t, key);
  
//--------------------//
// pthread_key_delete //
//--------------------//
  
_syscall1 (int, pthread_key_delete, pthread_key_t, key);

//---------------//
// pthread_yield //
//---------------//

void pthread_yield (void) {
  long __res;
  __syscall0 (pthread_yield_nr, __res);
}
