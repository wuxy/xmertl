/*
 * $FILE: irqs.c
 *
 * IRQs related functions
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

#include <linux_inc.h>
#include <domain.h>
#include <irqs.h>

unsigned long _restore_event_stack_code = 0;

//---------//
// dummy_f //
//---------//

// dummy_f is an useless function,
// but we need a valid address to create
// a fake stack
static void dummy_f (void) {
  sync_events ();
  enable_events_flag (xm_root_domain);
}

//------------------//
// sync_root_events //
//------------------//

/* 
   XXX: This function should be only called when 
   xm_current_domain == xm_root_domain
*/

static void sync_root_events (void) {
  unsigned long hw_flags, event;
  volatile bitmap_t pending;
  struct pt_regs dummy_regs = FAKE_REGS;

  hw_save_flags_and_cli(&hw_flags);

  pending = xm_root_domain -> events -> pending_events;
  xm_root_domain -> events -> pending_events &= 
    xm_root_domain -> events -> masked_events;

  // Executing all pending events
  while ((event = get_next_set_bit_and_clear 
	  (pending, xm_root_domain -> events -> masked_events)) != -1) {
    // An event  pending when there  is not any  handler installed
    // catching it??? it must be an error
    //assert (xm_root_domain -> event_handler [event]);
    if (!xm_root_domain -> events -> event_handler [event]) continue;
      // Before executing an event handler, events' flag is disabled	
      // and the event is masked	
    set_bit (xm_root_domain -> events -> masked_events, event);
    disable_events_flag (xm_root_domain);
    SET_PT_REG_IRQ (dummy_regs, event);
    hw_sti ();
    // Here, the event handler is executed always
    // with the same conditions, that is, event flag disabled
    // and the executed event masked as well
    (*xm_root_domain -> events -> event_handler[event]) 
      (event, &dummy_regs);
    hw_cli();
    // Events flag is automatically enabled (iret emulation)
    enable_events_flag (xm_root_domain);
  }
    
  hw_restore_flags (hw_flags);
}

//--------------------//
// sync_domain_events //
//--------------------//

/* 
   XXX: This function should be only called when 
   xm_current_domain != xm_root_domain
*/

static void sync_domain_events (void) {
  unsigned long hw_flags, event;
  volatile bitmap_t pending;
  struct pt_regs dummy_regs = FAKE_REGS;

  if (get_domain_state (xm_current_domain) == DOMAIN_FINISHED) return;

  hw_save_flags_and_cli(&hw_flags);
  
  pending = xm_current_domain -> events -> pending_events;
  xm_current_domain -> events -> pending_events &= 
    xm_current_domain -> events -> masked_events;

  // Executing all pending events
  while ((event = get_next_set_bit_and_clear 
	  (pending, xm_current_domain -> events -> masked_events)) != -1) {
    // An event  pending when there  is not any  handler installed
    // catching it??? it must be an error
    //assert (xm_current_domain -> event_handler [event]);
    if (!xm_current_domain -> events -> event_handler [event]) continue;
    // Before executing an event handler, events' flag is disabled
    // and the event is masked
    set_bit (xm_current_domain -> events -> masked_events, event);
    disable_events_flag (xm_current_domain);
    SET_PT_REG_IRQ (dummy_regs, event);
    hw_sti ();
    // Here, the event handler is executed always
    // with the same conditions, that is, event flag disabled
    // and the executed event masked as well
    
    (*xm_current_domain -> events -> event_handler[event]) 
      (event, &dummy_regs);
    hw_cli ();
    
    // Events flag is automatically enabled (iret emulation)
    enable_events_flag (xm_current_domain);
  }
 
  hw_restore_flags (hw_flags);
}

//-------------//
// sync_events //
//-------------//

// sync_event calls the event handlers
void sync_events (void) {
  unsigned long hw_flags;

  hw_save_flags_and_cli(&hw_flags);
  
  if (is_events_flag_enabled (xm_current_domain) && 
       is_some_bit_set (xm_current_domain -> events -> pending_events)) {
    if (xm_current_domain == xm_root_domain)
      sync_root_events ();
    else
      sync_domain_events ();
    // Events flag is automatically enabled (iret emulation)
    //enable_events_flag (xm_root_domain);
  }
  
  hw_restore_flags (hw_flags);
}

//----------------//
// build_user_irq //
//----------------//
/*
void build_user_irq (struct pt_regs regs) {
  unsigned long event;
  volatile bitmap_t pending;
  
  if (//xm_current_domain != xm_root_domain &&
      is_events_flag_enabled (xm_current_domain) && 
      is_some_bit_set (xm_current_domain -> events -> pending_events)) {
    pending = xm_current_domain -> events -> pending_events;
    xm_current_domain -> events -> pending_events &= 
    xm_current_domain -> events -> masked_events;

    // Executing all pending events
    while ((event = get_next_set_bit_and_clear 
	    (pending, xm_current_domain -> events -> masked_events)) != -1) {
      // An event  pending when there  is not any  handler installed
      // catching it??? it must be an error
      //assert (xm_current_domain -> event_handler [event]);
      if (!xm_current_domain -> events -> event_handler [event]) continue;
      // Before executing an event handler, events' flag is disabled
      // and the event is masked
      set_bit (xm_current_domain -> events -> masked_events, event);
      //setup_uirq (&regs, event, 
      //xm_current_domain -> events -> event_handler [event]);
    }

    
    // Events flag is automatically enabled (iret emulation)
    enable_events_flag (xm_current_domain);
  }
}
*/
//-------------------//
// linux_irq_handler //
//-------------------//

extern fastcall unsigned int do_IRQ (struct pt_regs *regs);
void linux_irq_handler (int irq, struct pt_regs *regs) {
  ((fastcall unsigned int (*) (struct pt_regs *))XM_root_func.__do_IRQ)(regs);
}
