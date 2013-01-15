/*
 * $FILE: syscalls.h
 *
 * XM's syscall table
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

#ifndef _XM_SYSCALLS_H_
#define _XM_SYSCALLS_H_

#define SYSCALL_ENTRIES 15

typedef void *syscall_handler_t;
extern syscall_handler_t syscall_table [SYSCALL_ENTRIES];

enum {
  exit_domain_nr,
#define exit_domain_nr exit_domain_nr
  load_domain_nr,
#define load_domain_nr load_domain_nr
  unload_domain_nr,
#define unload_domain_nr unload_domain_nr
  suspend_domain_nr,
#define suspend_domain_nr suspend_domain_nr
  sync_events_nr,
#define sync_events_nr sync_events_nr
  pass_event_nr,
#define pass_event_nr pass_event_nr
  get_time_nr,
#define get_time_nr get_time_nr
  set_timer_nr,
#define set_timer_nr set_timer_nr
  get_cpu_khz_nr,
#define get_cpu_khz_nr get_cpu_khz_nr
  write_scr_nr,
#define write_scr_nr write_scr_nr
  enable_hwirq_nr,
#define enable_hwirq_nr enable_hwirq_nr
  dev_ctl_nr,
#define dev_ctl_nr dev_ctl_nr
  shm_open_nr,
#define shm_open_nr shm_open_nr
  shm_unlink_nr,
#define shm_unlink_nr shm_unlink_nr		  
  xmtrace_user_nr
#define xmtrace_user_nr xmtrace_user_nr

};

#endif
