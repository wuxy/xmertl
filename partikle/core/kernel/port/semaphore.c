/*
 * $FILE: semaphore.c
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
 
#include <config.h>
#include <linkage.h>
#include <errno.h>
#include <pthread.h>
#include <pthread_list.h>
#include <signal.h>
#include <stddef.h>
#include <sched.h>
#include <time.h>
#include <arch/processor.h>
#include <semaphore.h>

//--------------//
// sem_init_sys //
//--------------//

asmlinkage int sem_init_sys (sem_t * sem, int pshared, unsigned value){
  if (pshared) {
    SET_ERRNO (ENOSYS);
    return -1;
  }
  
  if (sem) {
    *sem = (sem_t) {value: value, sem_list: 0};
  } else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  return 0;
}

//-----------------//
// sem_destroy_sys //
//-----------------//

asmlinkage int sem_destroy_sys(sem_t * sem){

  if (!sem){
    SET_ERRNO (EINVAL);
    return -1;
  }
  
// fail if there're suspended processes
  if (sem -> sem_list){
  	SET_ERRNO (EBUSY);
  	return -1;
  }
  return 0;
}

//------------------//
// sem_getvalue_sys //
//------------------//

asmlinkage int sem_getvalue_sys(sem_t * sem, int * sval){

  if (!sem){
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  *sval = sem -> value;
  return 0;
}

//--------------//
// sem_wait_sys //
//--------------//

asmlinkage int sem_wait_sys(sem_t * sem){
  int flags;
  pthread_t thread;
  
  if (!sem) {
  	SET_ERRNO(EINVAL);
  	return -1;
  }
  
  hw_save_flags_and_cli (flags);
  if (sem -> value == 0) {
    thread = current_thread;
    suspend_thread (current_thread);
    insert_pthread_list (&sem -> sem_list, &current_thread -> sem_list);
    scheduling ();
  }
  sem -> value = sem -> value - 1;
  hw_restore_flags(flags);
  return 0;
}

//-----------------//
// sem_trywait_sys //
//-----------------//

asmlinkage int sem_trywait_sys(sem_t * sem){
  int flags;
  
  if (!sem) {
  	SET_ERRNO(EINVAL);
  	return -1;
  }
  
  hw_save_flags_and_cli (flags);
  if (sem -> value == 0){
    SET_ERRNO(EAGAIN);
    hw_restore_flags(flags);
    return -1;
  } 
  
  sem -> value = sem -> value -1;
  hw_restore_flags(flags);
  return 0;
}

//-------------------//
// sem_timedwait_sys //
//-------------------//

asmlinkage int sem_timedwait_sys (sem_t *sem, const struct timespec * abstime) {
  int flags = 0, time_error = 0;
  duration_t d, r;
  
  if (!sem || !abstime){
  	SET_ERRNO(EINVAL);
  	return -1;
  }
  
  if (sem -> value > 0) {
    sem -> value = sem -> value - 1;
    if (sem -> value > 0)
       return 0;
  }

  if (abstime -> tv_nsec < 0 || abstime -> tv_nsec >= NSECS_PER_SEC){
    SET_ERRNO(EINVAL);
    return -1;
  }
  
  d = abstime -> tv_sec * NSECS_PER_SEC + abstime -> tv_nsec  
    - realtime_clock->gettime_nsec ();
  
  hw_save_flags_and_cli (flags);

  // enable asynchronous cancellation. required by the standard
  insert_pthread_list (&sem -> sem_list, &current_thread -> sem_list);
  r = sleep_for (d);
  remove_pthread_list (&sem -> sem_list, &current_thread -> sem_list);
  // disable asynchronous cancellation
    
  if (sem -> value > 0)
    sem -> value = sem -> value - 1;

  hw_restore_flags (flags);

  if (time_error) {
    SET_ERRNO (ETIMEDOUT);
    return -1;
  }

  return 0;
}

//--------------//
// sem_post_sys //
//--------------//

asmlinkage int sem_post_sys(sem_t * sem){
  int flags;
  pthread_t thread;
  
  if (! sem) {
    SET_ERRNO(EINVAL);
    return -1;
  }
  
  hw_save_flags_and_cli (flags);
  if (sem -> value == 0) {
    if ((thread = remove_head_pthread_list (&sem -> sem_list))){
      activate_thread(thread);
      scheduling ();
    }
  }
  
  if (sem -> value < SEM_VALUE_MAX)
    sem -> value = sem -> value + 1;
  else {
    SET_ERRNO(ERANGE);
    hw_restore_flags(flags);
   return -1;
  }

  hw_restore_flags(flags);
  return 0;	
}
