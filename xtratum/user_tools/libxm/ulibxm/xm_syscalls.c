/*
 * $FILE: xm_syscalls.c
 *
 * XM's user lib
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

#include <arch/syscalls.h>
#include <xm/syscalls.h>
#include <xm_syscalls.h>

#include <xm/config.h>
#include <xm/events.h>

event_handling_t *event_handling;
/*unsigned long *heap_addr;
unsigned long heap_size;*/

//-------------//
// exit_domain //
//-------------//

void exit_domain (int status) {
  int res;
  __syscall1(status, exit_domain_nr, res);
}

//-------------//
// load_domain //
//-------------//

_syscall3 (int, load_domain, char *, dname, int, prio, domain_image_t *, dimg);

//---------------//
// unload_domain //
//---------------//

_syscall1 (int, unload_domain, int, id);

//----------------//
// suspend_domain //
//----------------//

void suspend_domain (unsigned long dev, unsigned long cmd) {
  int res;
  __syscall2(dev, cmd, suspend_domain_nr, res);
}

//------------//
// pass_event //
//------------//

void pass_event (int event) {
  int res;
  __syscall1(event, pass_event_nr, res);
}

//-------------//
// sync_events //
//-------------//

static void sync_events (void) {
  int res;
  __syscall0(sync_events_nr, res);
}

//-----------//
// write_scr //
//-----------//

_syscall2 (int, write_scr, char *, buffer, int, length);


//---------------------//
// enable_events_flags //
//---------------------//

void enable_events_flag (void) {
  if (event_handling)
    event_handling -> event_flags |= EVENTS_ENABLED;
  if (event_handling && event_handling -> pending_events)
    sync_events();
}

//---------------------//
// disable_events_flag //
//---------------------//

void disable_events_flag (void) {
  if (event_handling)
    event_handling -> event_flags &= ~(EVENTS_ENABLED);
}

//------------------------//
// is_events_flag_enabled //
//------------------------//

int is_events_flag_enabled (void) {
  if (event_handling)
    return (event_handling -> event_flags & EVENTS_ENABLED);
  return 0;
}

//------------//
// mask_event //
//------------//

int mask_event (int event) {
  if (event < 0 || event >= MAX_EVENTS) return -BAD_EVENT_NM;
  
  if (event_handling)
    set_bit (event_handling -> masked_events, event);
  
  return 0;
}

_syscall1(int, enable_hwirq, int, irq);

//--------------//
// unmask_event //
//--------------//

int unmask_event (int event) {
  if (event < 0 || event >= MAX_EVENTS) return -BAD_EVENT_NM;
  
  if (event_handling)
    clear_bit (event_handling -> masked_events, event);
  
  enable_hwirq (event);
  
  if (!is_events_flag_enabled () &&
      ~event_handling -> masked_events & event_handling -> pending_events)
    sync_events ();
  return 0;
}

//-----------------//
// set_mask_events //
//-----------------//

void set_mask_events (bitmap_t *new, bitmap_t *old) {
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

int install_event_handler (int event, handler_t h) {
  if (event < 0 || event >= MAX_EVENTS) return -BAD_EVENT_NM;
  
  event_handling -> event_handler [event] = h;

  if (h)
    set_bit (event_handling -> intercepted_events, event);
  else
    clear_bit (event_handling -> intercepted_events, event);

  set_bit (event_handling -> masked_events, event);
  enable_events_flag ();

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

/* dev_ctl */

_syscall3(int, dev_ctl, unsigned long, dev, unsigned long, cmd, void *, arg);

/* shm_open */

_syscall3(int, shm_open, const char *, name, int, flag, unsigned int, mode);

/* shm_unlink */

_syscall1(int, shm_unlink, const char *, name);

/* xmtrace_user */

_syscall2(void, xmtrace_user, int, event_id, long, event_data);

