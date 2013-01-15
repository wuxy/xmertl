/*
 * $FILE: syscalls.c
 *
 * System calls
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

#include <fs.h>
#include <irqs.h>
#include <pthread.h>
#include <syscalls.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sysmemory.h>
#include <semaphore.h>
#include <rtl.h>
#include <arch/io.h>

syscall_t syscall_table [NR_SYSCALLS] = {
  exit_sys,  // 0
  open_sys,
  close_sys,
  read_sys,
  write_sys,
  lseek_sys,  
  ioctl_sys,
  mmap_sys,
  munmap_sys,
  pthread_exit_sys,
  pthread_create_sys, // 10
  pthread_join_sys,
  pthread_detach_sys,
  pthread_cleanup_pop_sys,
  pthread_cleanup_push_sys,
  pthread_once_sys, // 15
  pthread_cancel_sys,
  pthread_setcancelstate_sys,
  pthread_setcanceltype_sys,
  pthread_self_sys,
  pthread_getschedparam_sys, // 20
  pthread_setschedparam_sys,
  pthread_mutex_init_sys,
  pthread_mutex_destroy_sys,
  pthread_mutex_lock_sys,
  pthread_mutex_timedlock_sys, // 25
  pthread_mutex_trylock_sys,
  pthread_mutex_unlock_sys,
  pthread_cond_init_sys,
  pthread_cond_destroy_sys,
  pthread_cond_signal_sys, // 30
  pthread_cond_broadcast_sys,
  pthread_cond_wait_sys,
  pthread_cond_timedwait_sys,
  pthread_key_create_sys,
  pthread_setspecific_sys,
  pthread_getspecific_sys,
  pthread_key_delete_sys,
  pthread_yield_sys,
  time_sys,
  gettimeofday_sys, // 40
  usleep_sys,
  nanosleep_sys,
  clock_settime_sys,
  clock_gettime_sys,
  clock_getres_sys,
  sigaction_sys,
  sigsuspend_sys,
  sigwait_sys,
  sigpending_sys,
  pthread_kill_sys, // 50
  pthread_sigmask_sys,
  _get_errno_sys,
  _set_errno_sys,
  install_irq_handler_sys,
  install_trap_handler_sys,
  hw_disable_irq_sys,
  hw_enable_irq_sys,
  hw_ack_irq_sys,
  hw_end_irq_sys,
  hw_cli_sys, // 60
  hw_sti_sys,
  hw_restore_flags_sys,
  hw_save_flags_sys,
  hw_save_flags_and_cli_sys,
  ualloc_sys,
  ufree_sys,
  sem_init_sys,
  sem_destroy_sys,
  sem_getvalue_sys,
  sem_wait_sys, // 70
  sem_trywait_sys,
  sem_timedwait_sys,
  sem_post_sys,
  pthread_delete_np_sys,
  pthread_setperiod_np_sys,
  pthread_getperiod_np_sys,
  iopl_sys, // 77
  timer_create_sys,
  timer_delete_sys,
  timer_settime_sys,
  timer_gettime_sys,
  timer_getoverrun_sys
};
