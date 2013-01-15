/*
 * $FILE: xmsyscalls.c
 *
 * XM's system calls
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
#include <arch/xm_syscalls.h>
#include <stddef.h>
#include <bitmap.h>
#include "xmsyscalls.h"

event_handling_t *event_handling;
/*unsigned long *xm_heap_addr;
  unsigned long xm_heap_size;*/

//-------------//
// exit_domain //
//-------------//

void xm_exit_domain (int status) {
  int res;
  __syscall1(status, exit_domain_nr, res);
}

//----------------//
// suspend_domain //
//----------------//

void xm_suspend_domain (void) {
  int res;
  __syscall0(suspend_domain_nr, res);
}

//------------//
// pass_event //
//------------//

void xm_pass_event (int event) {
  int res;
  __syscall1(event, pass_event_nr, res);
}

//-------------//
// sync_events //
//-------------//

static void xm_sync_events (void) {
  int res;
  __syscall0(sync_events_nr, res);
}

//-----------//
// write_scr //
//-----------//

_syscall2 (int, write_scr, char *, buffer, int, length);
/*int xm_write_scr (char *b, int length)  {
  return 0;
}*/

//---------------------//
// enable_events_flags //
//---------------------//

void xm_enable_events_flag (void) {
  if (event_handling)
    event_handling -> event_flags |= EVENTS_ENABLED;
  if (event_handling && event_handling -> pending_events)
    xm_sync_events();
}

//---------------------//
// disable_events_flag //
//---------------------//

void xm_disable_events_flag (void) {
  if (event_handling)
    event_handling -> event_flags &= ~(EVENTS_ENABLED);
}

//------------------------//
// is_events_flag_enabled //
//------------------------//

int xm_is_events_flag_enabled (void) {
  if (event_handling)
    return (event_handling -> event_flags & EVENTS_ENABLED);
  return 0;
}

//------------//
// mask_event //
//------------//

int xm_mask_event (int event) {
  if (event < 0 || event >= MAX_EVENTS) return -1;
  
  if (event_handling)
    event_handling->masked_events=
      setbit (event_handling -> masked_events, event);
  
  return 0;
}

//--------------//
// unmask_event //
//--------------//

int xm_unmask_event (int event) {
  if (event < 0 || event >= MAX_EVENTS) return -1;
  
  if (event_handling)
      event_handling->masked_events=
	clearbit (event_handling -> masked_events, event);
  if (!xm_is_events_flag_enabled () &&
      ~event_handling -> masked_events & event_handling -> pending_events)
    xm_sync_events ();
  return 0;
}

//-----------------//
// set_mask_events //
//-----------------//

void xm_set_mask_events (bitmap_t *new, bitmap_t *old) {
  if (event_handling) {
    if (old)
      *old = event_handling -> masked_events;
    if (new)
      event_handling -> masked_events = *new;
  }
}

//-----------------------//
// install_event_handler //
//-----------------------//

int xm_install_event_handler (int event, handler_t h) {
  if (event < 0 || event >= MAX_EVENTS) return -1;

  if (event) return 0;
  
  event_handling -> event_handler [event] = h;

  if (h)
    event_handling -> intercepted_events=
      setbit (event_handling -> intercepted_events, event);
  else
    event_handling -> intercepted_events=
      clearbit (event_handling -> intercepted_events, event);

  event_handling->masked_events=
    setbit (event_handling -> masked_events, event);
  xm_enable_events_flag ();

  return 0;
}

//----------//
// get_time //
//----------//

_syscall1 (int, get_time, struct xmtimespec *, t);

//-----------//
// set_timer //
//-----------//

_syscall2(int, set_timer, struct xmitimerval *, req, 
	  struct xmitimerval *, rem);

//-------------//
// get_cpu_khz //
//-------------//

_syscall0(unsigned long, get_cpu_khz);

//-----------------//
// get_xmheap_addr //
//-----------------//

unsigned long *get_xmheap_addr(void) {
  return event_handling->heap;
}

//-----------------//
// get_xmheap_size //
//-----------------//

unsigned long get_xmheap_size(void) {
  return event_handling->heap_size;
}


/* dev_ctl */

_syscall3(int, dev_ctl, unsigned long, dev, unsigned long, cmd, void *, arg);

/* shm_open */

_syscall3(int, shm_open, const char *, name, int, flag, unsigned int, mode);

/* shm_unlink */

_syscall1(int, shm_unlink, const char *, name);

/*#ifdef CONFIG_XTRATUM_FIFO

_syscall3(int, fifo_read, int, channel, char *, buff, int ,size);
_syscall3(int, fifo_write, int, channel, const char *, buff, int ,size);

#endif

*/
