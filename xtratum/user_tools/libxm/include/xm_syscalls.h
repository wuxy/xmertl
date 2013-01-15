/*
 * $FILE: xm_syscalls.h
 *
 * XM's syscall definitions
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

#ifndef _XM_LIB_SYSCALLS_H_
#define _XM_LIB_SYSCALLS_H_

#include <xm/arch/domain.h>
#include <xm/events.h>
#include <xm/errno.h>
#include <xm/timer.h>

extern unsigned long heap_size;
extern unsigned long *heap_addr;
extern event_handling_t *event_handling;

extern void exit_domain (int status);
extern int load_domain (char *dname, int prio, domain_image_t *dimg);
extern int unload_domain (int id);
extern void suspend_domain (unsigned long dev, unsigned long cmd);
extern int install_event_handler (int event, handler_t h);
extern void enable_events_flag (void);
extern void disable_events_flag (void);
extern int is_events_flag_enabled(void);
extern int mask_event (int event);
extern int unmask_event (int event);
extern void set_mask_events (bitmap_t *new, bitmap_t *old);
//extern void sync_events (void);
extern void pass_event (int event);
extern int write_scr (char *buffer, int length);
extern int get_time (struct xmtimespec *t);
extern int set_timer (struct xmitimerval *, struct xmitimerval *);
extern unsigned long get_cpu_khz (void);
extern int shm_unlink(const char *name);
extern int shm_open(const char *name, int flag, unsigned int mode);
extern int dev_ctl(unsigned long dev, unsigned long cmd, void *arg);
extern void xmtrace_user(int event_id, long event_data);
#endif
