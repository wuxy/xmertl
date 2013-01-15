/*
 * $FILE: types.h
 *
 * Types
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

#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef _KERNEL_
#include <config.h>
struct pthread_struct;
typedef struct pthread_struct *pthread_t;
#include <pthread_list.h>
#else
typedef unsigned long *pthread_t;
typedef unsigned long *pthread_list_t;
#endif

typedef int clockid_t;
typedef int timer_t;

struct sched_param {
  int policy;                 // scheduling policy
  int sched_priority;
  duration_t deadline;
  duration_t period;          // Periodic thread (needed for rtl.h)
};

// pthread_once_t is an atomic variable
typedef struct pthread_once_struct {
  volatile int init;
} pthread_once_t;

typedef struct pthread_attr_struct {
  unsigned int stack_size;
  void *stack_addr;
  int policy;
  struct sched_param sched_param;
  int detachstate;
} pthread_attr_t;

typedef struct pthread_mutex_attr_struct {
  int prio_protection;
  int prio;
  int type;
  int owner_prio;
} pthread_mutexattr_t;

typedef struct pthread_mutex_struct {
  int magic_number;
  int counter;
  pthread_mutexattr_t attr;
  pthread_t owner;
  pthread_list_t *mutex_list;

  /* Used to trace the mutex, only when tracing is enabled */
#ifdef CONFIG_PORT_DEVTRACE
    int trace_id;
#endif
} pthread_mutex_t;

typedef struct pthread_cond_attr_struct {
  int dummy;
} pthread_condattr_t;

typedef struct pthread_cond_struct {
  pthread_list_t *cond_list;

  /* Used to trace the cond, only when tracing is enabled */
#ifdef CONFIG_PORT_DEVTRACE
    int trace_id;
#endif
} pthread_cond_t;

typedef int pthread_key_t;

typedef int pid_t;

/*
 * We have up to 32 signals
 */

typedef struct {
  unsigned long sig;
} sigset_t;

#ifdef _KERNEL_

#include <stack.h>
#include <hwtimers.h>
#include <arch/arch_types.h>

struct cleanup_routine {
  void (*routine) (void *);
  void *arg;
};

struct common_key_struct {
  char in_use;
  void (*destructor) (void *);
};

struct key_inf {
  void *i;
};

struct ptimer_struct {
  hwtime_t next_activation;
  int signal;
  int retsig;
  clockid_t clockid;
};

struct ktimer_struct;
struct pthread_struct {
  unsigned long *stack; /* Hardcoded, don't change it */

  /*
   * Thread flag bits
   * 
   * 10     -> Activate on signal MASK 0x400
   * 9 .. 6 -> Used policy        MASK 0x3c0
   * 5      -> Cancel type        MASK 0x20
   * 4      -> Cancel state       MASK 0x10
   * 3      -> Detach state       MASK 0x8
   * 2..0   -> Current state      MASK 0x7
   */
  
  int thread_flags;

  /*
   * Stack's lowest address and size
   */
  stack_t stack_info;

  /*
   * Scheduling info
   */

  struct sched_param sched_param;

  /*
   * Used to verify any pthread pointer
   */
  
  unsigned long magic_number;

  /*
   * Error number of the last called function
   */

  int errno;
  
  /*
   * When a thread exits via pthread_exit, exit_value stores its
   * returning value
   */

  void *exit_value;

  /*
   * Pointer to a thread who is waiting this thread finalisation
   */

  struct pthread_struct *joining_thread;

  /*
   * Timer use to sleep the thread
   */

  struct ktimer_struct *sleep_ktimer;

  /*
   * CPU CLOCK
   */

  duration_t used_time;
  duration_t starting_time;

  /*
   * Dealing with signals ...
   */

  sigset_t sigpending;
  sigset_t sigmask;

  /*
   * Used to trace the thread, only when tracing is enabled
   */

#ifdef CONFIG_PORT_DEVTRACE
  int trace_id;
#endif

  /*
   * cleanup handlers
   */
  struct cleanup_routine cleanup_routine_table [MAX_CLEANUP_HANDLERS];
  int top_cr_table;

  /*
   * Lists where this thread can be inserted
   */
  pthread_list_t thread_list, mutex_list, cond_list, sem_list;

  /* 
   * Pointers to manage the thread inside of the scheduler's structure
   */

  struct pthread_struct *next_matrix, *prev_matrix;
  int ss_i, ss_j;

  /* Here local thread information is stored */
  struct key_inf tsd [MAX_PTHREAD_KEYS];
};



#endif
#endif
