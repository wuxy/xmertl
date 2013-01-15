/*
 * $FILE: start.c
 *
 * Main XtratuM's routines
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
#include <irqs.h>
#include <hwtimer.h>
#include <timer.h>
#include <bitmap.h>
#include <domain.h>
#include <sched.h>
#include <arch/cpu.h>

void rtl_trace_default(int event_id, long event_data, void *eip) {}
void (*rtl_trace)(int event_id, long event_data, void *eip) = rtl_trace_default;
void rtl_trace2(int event_id, long event_data) { rtl_trace(event_id, event_data, __builtin_return_address(0)); }
EXPORT_SYMBOL(rtl_trace_default);
EXPORT_SYMBOL(rtl_trace);
EXPORT_SYMBOL(rtl_trace2);

extern void create_xm_proc (void);
extern void remove_xm_proc (void);

//---------//
// xm_init //
//---------//

int __init xm_init (void) {
  unsigned long flags;
  int event;

  printk ("<XtratuM> Initialisating XtratuM\n");
  
  hw_save_flags_and_cli (&flags);

  xm_root_domain = __create_domain_node 
    ("Linux", min_priority());
  
  if (!xm_root_domain) {
    hw_restore_flags (flags);
    return -1;
  }

  xm_root_domain -> events = vmalloc (sizeof (event_handling_t));
  if (!xm_root_domain -> events) {
    hw_restore_flags (flags);
    return -1;
  }
  memset ((char *)xm_root_domain -> events, 0, sizeof (event_handling_t));

  set_bitmap (xm_root_domain -> events -> masked_events);
  enable_events_flag (xm_root_domain);
  set_domain_state(xm_root_domain, DOMAIN_ACTIVE);
  
  for (event = 0; event < 16; event ++) {
    set_bit (xm_root_domain -> events -> intercepted_events, event);
    clear_bit (xm_root_domain -> events -> masked_events, event);
    xm_root_domain -> events -> event_handler [event] = linux_irq_handler;
  }
  
  xm_current_domain = xm_root_domain;
  insert_domain_list (xm_root_domain, &xm_domain_list);

  arch_setup ();
  
  init_timers ();

  hw_irq_takeover ();
  create_xm_proc ();
  
  hw_restore_flags (flags);
  
  return 0;
}

//-------------------------//
// destroy_domain_callback //
//-------------------------//

static void destroy_domain_callback (domain_t *d) {
  if (d) {
    if (d != xm_root_domain) {
      destroy_page_directory (d->pd, mm_release_page);
      if (d->sstack_st)
	vfree (d->sstack_st);
      timer_remove_domain (d);
    } else {
      if (d -> events)
	vfree (d -> events);
    }
  }
}

//------------//
// xm_cleanup //
//------------//

void __exit xm_cleanup (void) 
{
	unsigned long flags;

	hw_save_flags_and_cli (&flags);
	remove_xm_proc ();
	hw_irq_giveup ();
	uninit_timers ();
	destroy_domain_list (&xm_domain_list, destroy_domain_callback);

	hw_restore_flags (flags);
}


module_init(xm_init);
module_exit(xm_cleanup);

MODULE_AUTHOR("Miguel Masmano <mimastel@doctor.upv.es>");
MODULE_DESCRIPTION("XtratuM");
MODULE_LICENSE("GPL");
