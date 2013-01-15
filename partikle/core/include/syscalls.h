/*
 * $FILE: syscalls.h
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

#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#ifdef _KERNEL_

#define NR_SYSCALLS 83

#endif

#ifndef __ASSEMBLY__

#ifdef _KERNEL_
typedef void *syscall_t;

extern syscall_t syscall_table [NR_SYSCALLS];
#endif

/*
 *  SYSCALL TABLE NUMBERS
 */

enum {
  exit_nr,
#define exit_nr exit_nr           // 0
  open_nr,
#define open_nr open_nr
  close_nr,
#define close_nr close_nr
  read_nr,
#define read_nr read_nr
  write_nr,
#define write_nr write_nr
  lseek_nr,
#define lseek_nr lseek_nr         // 5
  ioctl_nr,
#define ioctl_nr ioctl_nr
  mmap_nr,
#define mmap_nr mmap_nr
  munmap_nr,
#define munmap_nr munmap_nr
  pthread_exit_nr,
#define pthread_exit_nr pthread_exit_nr
  pthread_create_nr,
#define pthread_create_nr pthread_create_nr      // 10
  pthread_join_nr,
#define pthread_join_nr pthread_join_nr
  pthread_detach_nr,
#define pthread_detach_nr pthread_detach_nr
  pthread_cleanup_pop_nr,
#define pthread_cleanup_pop_nr pthread_cleanup_pop_nr
  pthread_cleanup_push_nr,
#define pthread_cleanup_push_nr pthread_cleanup_push_nr
  pthread_once_nr,
#define pthread_once_nr pthread_once_nr             // 15
  pthread_cancel_nr,
#define pthread_cance_nr pthread_cancel_nr
  pthread_setcancelstate_nr,
#define pthread_setcancelstate_nr pthread_setcancelstate_nr
  pthread_setcanceltype_nr,
#define pthread_setcanceltype_nr pthread_setcanceltype_nr
  pthread_self_nr,
#define pthread_self_nr pthread_self_nr
  pthread_getschedparam_nr,
#define pthread_getschedparam_nr pthread_getschedparam_nr // 20
  pthread_setschedparam_nr,
#define pthread_setschedparam_nr pthread_setschedparam_nr
  pthread_mutex_init_nr,
#define pthread_mutex_init_nr pthread_mutex_init_nr
  pthread_mutex_destroy_nr,
#define pthread_mutex_destroy_nr pthread_mutex_destroy_nr
  pthread_mutex_lock_nr,
#define pthread_mutex_lock_nr pthread_mutex_lock_nr
  pthread_mutex_timedlock_nr,
#define pthread_mutex_timedlock_nr pthread_mutex_timedlock_nr  // 25
  pthread_mutex_trylock_nr,
#define pthread_mutex_trylock_nr pthread_mutex_trylock_nr
  pthread_mutex_unlock_nr,
#define pthread_mutex_unlock_nr pthread_mutex_unlock_nr
  pthread_cond_init_nr,
#define pthread_cond_init_nr pthread_cond_init_nr
  pthread_cond_destroy_nr,
#define pthread_cond_destroy_nr pthread_cond_destroy_nr
  pthread_cond_signal_nr,
#define pthread_cond_signal_nr pthread_cond_signal_nr  // 30
  pthread_cond_broadcast_nr,
#define pthread_cond_broadcast_nr pthread_cond_broadcast_nr
  pthread_cond_wait_nr,
#define pthread_cond_wait_nr pthread_cond_wait_nr
  pthread_cond_timedwait_nr,
#define pthread_cond_timedwait_nr pthread_cond_timedwait_nr
  pthread_key_create_nr,
#define pthread_key_create_nr pthread_key_create_nr
  pthread_setspecific_nr,
#define pthread_setspecific_nr pthread_setspecific_nr // 35
  pthread_getspecific_nr,
#define pthread_getspecific_nr pthread_getspecific_nr
  pthread_key_delete_nr,
#define pthread_key_delete_nr pthread_key_delete_nr
  pthread_yield_nr,
#define pthread_yield_nr pthread_yield_nr
  time_nr,
#define time_nr time_nr
  gettimeofday_nr,
#define gettimeofday_nr gettimeofday_nr // 40
  usleep_nr,
#define usleep_nr usleep_nr
  nanosleep_nr,
#define nanosleep_nr nanosleep_nr
  clock_settime_nr,
#define clock_settime_nr clock_settime_nr
  clock_gettime_nr,
#define clock_gettime_nr clock_gettime_nr
  clock_getres_nr,
#define clock_getres_nr clock_getres_nr
  sigaction_nr,
#define sigaction_nr sigaction_nr
  sigsuspend_nr,
#define sigsuspend_nr sigsuspend_nr
  sigwait_nr,
#define sigwait_nr sigwait_nr
  sigpending_nr,
#define sigpending_nr sigpending_nr
  pthread_kill_nr,
#define pthread_sigmask_nr pthread_sigmask_nr // 50
  pthread_sigmask_nr,
#define pthread_kill_nr pthread_kill_nr
  _get_errno_nr,
#define _get_errno_nr _get_errno_nr
  _set_errno_nr,
#define _set_errno_nr _set_errno_nr
  install_irq_handler_nr,
#define install_irq_handler_nr install_irq_handler_nr  
  install_trap_handler_nr,
#define install_trap_handler_nr install_trap_handler_nr
  hw_disable_irq_nr,
#define hw_disable_irq_nr hw_disable_irq_nr
  hw_enable_irq_nr,
#define hw_enable_irq_nr hw_enable_irq_nr
  hw_ack_irq_nr,
#define hw_ack_irq_nr hw_ack_irq_nr
  hw_end_irq_nr,
#define hw_end_irq_nr hw_end_irq_nr
  hw_cli_nr,
#define hw_cli_nr hw_cli_nr // 60
  hw_sti_nr,
#define hw_sti_nr hw_sti_nr
  hw_restore_flags_nr,
#define hw_restore_flags_nr hw_restore_flags_nr
  hw_save_flags_nr,
#define hw_save_flags_nr hw_save_flags_nr
  hw_save_flags_and_cli_nr,
#define hw_save_flags_and_cli_nr hw_save_flags_and_cli_nr
  ualloc_nr,
#define ualloc_nr ualloc_nr
  ufree_nr,
#define ufree_nr ufree_nr
  sem_init_nr,
#define sem_init_nr sem_init_nr
  sem_destroy_nr,
#define sem_destroy_nr sem_destroy_nr
  sem_getvalue_nr,
#define sem_getvalue_nr sem_getvalue_nr
  sem_wait_nr,
#define sem_wait_nr sem_wait_nr // 70
  sem_trywait_nr,
#define sem_trywait_nr sem_trywait_nr
  sem_timedwait_nr,
#define sem_timedwait_nr sem_timedwait_nr
  sem_post_nr,
#define sem_post_nr sem_post_nr
  pthread_delete_np_nr,
#define pthread_delete_np_nr pthread_delete_np_nr
  pthread_setperiod_np_nr,
#define pthread_setperiod_np_nr pthread_setperiod_np_nr
  pthread_getperiod_np_nr,
#define pthread_getperiod_np_nr pthread_getperiod_np_nr
  iopl_nr,
#define iopl_nr iopl_nr
  timer_create_nr,
#define timer_create_nr timer_create_nr
  timer_delete_nr,
#define timer_delete_nr timer_delete_nr
  timer_settime_nr,
#define timer_settime_nr timer_settime_nr // 80
  timer_gettime_nr,
#define timer_gettime_nr timer_gettime_nr
  timer_getoverrun_nr
#define timer_getoverrun_nr timer_getoverrun_nr
};

#endif
#endif
