/*
 * $FILE: sched.c
 *
 * PaRTiKle's scheduler
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

#include <irqs.h>
#include <pthread.h>
#include <pthread_list.h>
#include <sched.h>
#include <sched_struct.h>
#include <signal.h>
#include <stddef.h>
#include <string.h>
#include <tlsf.h>
#include <arch/processor.h>
#include <trace.h>

// Definitions

// Definition of the most important threads: The
// currently-in-execution thread and the idle_thread. 

// It is important to note that the idle thread will never inserterted
// within the scheduler structure, it will be scheduled just when no
// other thread is READY

static pthread_t idle_thread = NULL;
pthread_t current_thread = NULL;

// List of threads
static pthread_list_t *thread_list;

// The scheduler's structure
static active_threads_t active_threads;

//-----------------------//
// create_pthread_struct //
//-----------------------//

pthread_t create_pthread_struct (void) {
  pthread_t thread;

  if (!(thread = malloc_ex (sizeof (struct pthread_struct), memory_pool))) {
    return NULL;
  }

  // Zeroing the thread structure
  memset ((unsigned char *) thread, 0, sizeof (struct pthread_struct));

  thread -> magic_number = PTHREAD_MAGIC_NUMBER;
  SET_THREAD_STATE(thread,  SUSPENDED_THREAD);
  
  // Initialising list structures
  thread -> thread_list.this = thread -> mutex_list.this = 
    thread -> cond_list.this = thread -> sem_list.this = thread;
  
  /*
   * Every thread has available a ktimer for sleeping
   */
  if (!(thread -> sleep_ktimer = &timer_array  
	[alloc_ktimer (CLOCK_MONOTONIC, thread, action_wakeup)]))
    return NULL;
  
  /*
   * Only user signal mask is inherited from the creating thread
   */
  /*
   * By default no thread is masked
   */
  if (current_thread)
    thread -> sigmask = current_thread -> sigmask;

  // Inserting the thread into the thread_list
  insert_pthread_list (&thread_list, &thread -> thread_list);
  

#ifdef CONFIG_PORT_DEVTRACE
  thread -> trace_id = last_trace_id ++;
#endif

  return thread;
}

//-----------------------//
// delete_pthread_struct //
//-----------------------//

void delete_pthread_struct (pthread_t thread) {
  
  // Firstly, the thread is removed from the ready structure
  remove_ss (thread, &active_threads);

  // Secondly, the thread is removed from the thread list
  remove_pthread_list (&thread_list, &thread -> thread_list);

  // Releasing the stack
  dealloc_stack (&thread -> stack_info);

  // Releasing the thread
  free_ex (thread, memory_pool);
  
}

//---------------//
// creating idle //
//---------------//

int create_idle (void) {
  // Creating the idle thread structure
  if (!(idle_thread = create_pthread_struct ())) 
    return -1;
  
  current_thread = idle_thread;
  current_thread -> starting_time = monotonic_clock->gettime_nsec();
  return 0;
}

//--------------------//
// change_thread_prio //
//--------------------//

void change_thread_prio (pthread_t thread, int prio) {
  int flags;
  int prev_state;
  
  hw_save_flags_and_cli (flags);
  prev_state = GET_THREAD_STATE (thread);

  remove_ss (thread, &active_threads);
  thread -> sched_param.sched_priority = prio;
  
  if (prev_state == READY_THREAD)
    insert_ss (thread, thread -> sched_param.sched_priority,
	       &active_threads);

  hw_restore_flags (flags);
}

//----------------//
// suspend_thread //
//----------------//

void suspend_thread (pthread_t thread) {
  int flags;

  hw_save_flags_and_cli (flags);
  remove_ss (thread, &active_threads);
  if (GET_THREAD_STATE (thread) != FINISHED_THREAD)
    SET_THREAD_STATE (thread, SUSPENDED_THREAD);
  hw_restore_flags (flags);
}

//-----------------//
// activate_thread //
//-----------------//

void activate_thread (pthread_t thread) {
  int flags;
  
  hw_save_flags_and_cli (flags);
  if (GET_THREAD_STATE (thread) != FINISHED_THREAD && 
      GET_THREAD_STATE (thread) != READY_THREAD) {
    insert_ss (thread, thread -> sched_param.sched_priority, &active_threads);
    SET_THREAD_STATE (thread, READY_THREAD);
  }
  hw_restore_flags (flags);
}

//---------------//
// finish_thread //
//---------------//

void finish_thread (pthread_t thread) {
  int flags;

  hw_save_flags_and_cli (flags);
  remove_ss (thread, &active_threads);
  SET_THREAD_STATE (thread, FINISHED_THREAD);
  hw_restore_flags (flags);
}

//-------------------//
// pthread_yield_sys //
//-------------------//

asmlinkage int pthread_yield_sys (void) {
  int flags;
  
  hw_save_flags_and_cli (flags);
  remove_ss (current_thread, &active_threads);
  insert_ss_tail (current_thread, 
		  current_thread -> sched_param.sched_priority, 
		  &active_threads);
  hw_restore_flags (flags);
  scheduling ();

  return 0;
}

//------------//
// scheduling //
//------------//

void scheduling (void) {
  int flags;
  pthread_t new_thread;

  hw_save_flags_and_cli (flags);

  // When an interrupt is in-progress, the scheduler shouldn't be invoked
  if (irq_nesting_counter & OUTSIDE_IRQS) {
    irq_nesting_counter |= SCHED_PENDING;
    hw_restore_flags (flags);
    return;
  }

  new_thread = get_head_ss (NULL, &active_threads);

  if (!new_thread)
    new_thread = idle_thread;
  
  if (new_thread != current_thread) {
    current_thread -> used_time += (monotonic_clock->gettime_nsec() - 
				    current_thread -> starting_time);
    
#ifdef CONFIG_PORT_DEVTRACE
  log_event ((struct Traceevent) {
	etype: exece,
	id: current_thread->trace_id,
	time: monotonic_clock->gettime_nsec ()
  });
 
  log_event ((struct Traceevent) {
	etype: execb,
	id: new_thread->trace_id,
	time: monotonic_clock->gettime_nsec ()
  });
#endif
    context_switch (new_thread, &current_thread);
    current_thread -> starting_time = monotonic_clock->gettime_nsec();
  }

  irq_nesting_counter = 0;
  hw_restore_flags (flags);
}

//------------//
// init_sched //
//------------//

int init_sched (void) {
  init_ss (&active_threads);
  init_pthread_list (&thread_list);
  return 0;
}
