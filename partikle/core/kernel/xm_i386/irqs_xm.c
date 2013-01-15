/*
 * $FILE: irqs.c
 *
 * Hardware-dependent part of irqs
 * XM version
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

#include <irqs.h>
#include <bitmap.h>
#include <string.h>
#include <arch/xm_syscalls.h>
#include <arch/processor.h>

//--------//
// do_irq //
//--------//
extern void scheduling (void);
static void do_irq (int irqnr, void *context) {
  irq_nesting_counter ++;
  if (hw_irq_ctrl [irqnr].ack)
    hw_ack_irq (irqnr);

  if (irq_handler_table [irqnr] && 
      irq_handler_table [irqnr] != default_irq_handler)
    (*irq_handler_table [irqnr]) ((context_t *)context);
  else {  
    //default_irq_handler ((context_t *)context);
    xm_pass_event (irqnr);
  }

  if (hw_irq_ctrl [irqnr].end)
    hw_end_irq (irqnr);
  irq_nesting_counter --;
  if (irq_nesting_counter == SCHED_PENDING)
    scheduling ();   
}

//---------------//
// init_arch_irq //
//---------------//

int init_arch_irqs (void) {
  int irq;
  
  for (irq = 0; irq < IRQ_NR; irq ++) {
    hw_irq_ctrl [irq] = (hw_irq_ctrl_t){
      .enable = (void (*) (unsigned int))xm_unmask_event,
      .disable = (void (*) (unsigned int))xm_mask_event,
      .ack = (void (*) (unsigned int))xm_mask_event,
      .end = (void (*) (unsigned int))xm_unmask_event
    };
    xm_install_event_handler (irq, do_irq);
    hw_enable_irq (irq);
  }

  return 0;
}
