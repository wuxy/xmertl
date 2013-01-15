/*
 * $FILE: sched.c
 *
 * XM's scheduler
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
#include <bitmap.h>
#include <domain.h>
#include <sched.h>
#include <xmtrace.h>

#include <arch/cpu.h>

//----------------------//
// generic_trap_handler //
//----------------------//

extern void show_trap_info (int trap, struct pt_regs *regs);
void generic_trap_handler (int trap, struct pt_regs *regs) {
  show_trap_info (trap, regs);
  set_domain_state (xm_current_domain, DOMAIN_FINISHED);
  xm_sched();
}

//--------------//
// init_wrapper //
//--------------//

int init_wrapper (int (*entry_point) (void)) {
  //hw_save_stack (xm_current_domain -> sstack);
  /*hw_save_stack (xm_current_domain -> irq_sstack);*/

  /*load_sstack (xm_current_domain);*/
  load_pd (xm_current_domain -> pd);

#ifdef CONFIG_FP_SUPPORT
  init_fp();
#endif

  set_bitmap (xm_current_domain -> events -> masked_events);

  // Events' flag is disable by default
  disable_events_flag (xm_current_domain);

  /*init_ustack (xm_current_domain -> ustack, 
    xm_current_domain -> events_user_addr);*/

  //JUMP_USER_LEVEL (entry_point, xm_current_domain -> ustack);
  
  JUMP_XM_DOM(entry_point, xm_current_domain);
 // printk ("Saliendo\n");
  disable_events_flag (xm_current_domain);
  set_domain_state(xm_current_domain, DOMAIN_FINISHED);
  xm_sched ();
  
  // This point should be unreachable
  return 0;
}

//----------//
// xm_sched //
//----------//

int xm_sched (void) {
  domain_t *sel_domain;
  unsigned long hw_flags;

  set_tracer_flag();
  rtl_trace2(RTL_TRACE_SCHED_IN,0);

  //assert (!hw_are_interrupts_enabled ());
  hw_save_flags_and_cli (&hw_flags);

  for (sel_domain = xm_domain_list; sel_domain; 
       sel_domain = sel_domain -> next) {

    // Bypassing all not intercepted events to the next domain in the list
    if (sel_domain -> next) {
      if (get_domain_state(sel_domain) != DOMAIN_FINISHED) {
      	sel_domain -> next -> events -> pending_events |= 
	  (sel_domain -> events -> pending_events & ~sel_domain -> 
	   events -> intercepted_events);
	sel_domain -> events -> 
	  pending_events =  (sel_domain -> events -> pending_events & 
			     sel_domain -> events -> intercepted_events);
      } else {
        sel_domain -> next -> events -> pending_events |= 
	  sel_domain -> events -> pending_events;
	sel_domain -> events -> pending_events = 0;
      }
    }
 
    // pending but not intercepted  signals have already been cleared,
    // so next search is correct 
    if (get_domain_state(sel_domain) == DOMAIN_ACTIVE || 
	(get_domain_state(sel_domain) == DOMAIN_SUSPENDED && 
	 is_events_flag_enabled (sel_domain) &&
	 get_domain_pending_events(sel_domain -> events -> pending_events,
				   sel_domain -> events -> masked_events)))
      break;
  }
  // no selected  domain?, well it doesn't the  matter, XtratuM can
  // always try to execute the root domain, even if the root domain
  // would be suspended
  if (!sel_domain) sel_domain = xm_root_domain;

  if (sel_domain != xm_current_domain) {
    save_pd (xm_current_domain -> pd);
    
    //save_sstack (xm_current_domain);
    //load_sstack (sel_domain);

    // Caution,  recall that  after the  domain_context_switch, the
    // selected variable may be incorrect since the stack may
    // change
    domain_context_switch (sel_domain, &xm_current_domain);
  }

  set_domain_state(xm_current_domain, DOMAIN_ACTIVE);

  if (!is_events_flag_enabled (xm_current_domain)) {
    hw_restore_flags (hw_flags);
    return 0;
  }
  // Calling current domain's event handlers
  sync_events();
  
  hw_restore_flags (hw_flags);

  rtl_trace2(RTL_TRACE_SCHED_OUT,xm_current_domain->id);
  unset_tracer_flag();
  return 1;
}
