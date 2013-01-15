/*
 * $FILE: xm_syscalls.h
 *
 * Definition of the HAL interface
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

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <bitmap.h>

#define MAX_EVENTS 32
#define MAX_TRAPS 20

#define FIFO_COUNT 16

typedef void (*handler_t) (int, void *);

typedef struct event_handling_struct {
  volatile bitmap_t intercepted_events;
  volatile bitmap_t masked_events;
  volatile bitmap_t pending_events;

  handler_t event_handler [MAX_EVENTS];
  handler_t trap_handler [MAX_TRAPS];

  volatile unsigned long event_flags;
  unsigned long *heap;
  unsigned long heap_size;
  // XtratuM FIFO
  unsigned long fctladdr;
  unsigned long fdataddr[FIFO_COUNT];
} event_handling_t;

///////////////////// EVENTS ENABLED

#define EVENTS_ENABLED 0x10 // 1 bit (4)

struct xmtimespec {
  int tv_sec; /* Seconds.  */
  long int tv_nsec; /* Nanoseconds.  */
};

struct xmitimerval {
  struct xmtimespec it_interval; /* next value */
  struct xmtimespec it_value; /* current value */
};

/*
extern unsigned long *xm_heap_addr;
extern unsigned long xm_heap_size;
*/

extern unsigned long *get_xmheap_addr(void);
extern unsigned long get_xmheap_size(void);
extern void xm_exit_domain (int status);
extern void xm_suspend_domain (void);
extern int xm_install_event_handler (int event, handler_t h);
extern void xm_enable_events_flag (void);
extern void xm_disable_events_flag (void);
extern int xm_is_events_flag_enabled(void);
extern int xm_mask_event (int event);
extern int xm_unmask_event (int event);
extern void xm_set_mask_events (bitmap_t *new, bitmap_t *old);
extern void xm_pass_event (int event);
extern int xm_write_scr (char *buffer, int length);
extern int xm_get_time (struct xmtimespec *t);
extern int xm_set_timer (struct xmitimerval *, struct xmitimerval *);
extern unsigned long xm_get_cpu_khz (void);

extern int xm_dev_ctl(unsigned long dev, unsigned long cmd, void * arg);
extern int xm_shm_open(const char * name, int flag, unsigned int mode);
extern int xm_shm_unlink(const char * name);
#endif

