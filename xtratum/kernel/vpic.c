/*
 * $FILE: vpic.c
 *
 * Virtual PIC
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
#include <domain.h>
#include <arch/cpu.h>

//------------------------//
// virtual_irq_hw_startup //
//------------------------//

static unsigned int virtual_irq_hw_startup (unsigned int irq){
  unsigned long hw_flags;
  hw_save_flags_and_cli (&hw_flags);
  clear_bit (xm_root_domain -> events -> masked_events, irq);
  hw_restore_flags (hw_flags);
  return (hw_xpic [irq]->startup(irq));
  
  return 0;
}

//-------------------------//
// virtual_irq_hw_shutdown //
//-------------------------//

static void virtual_irq_hw_shutdown (unsigned int irq) {
  
  set_bit (xm_root_domain -> events -> masked_events, irq);
  if (hw_xpic [irq] &&
      hw_xpic [irq] -> shutdown)
    hw_xpic[irq] -> shutdown(irq);
}

//--------------------//
// virtual_irq_hw_ack //
//--------------------//

static void virtual_irq_hw_ack (unsigned int irq){
  return;
}

//-----------------------//
// virtual_irq_hw_enable //
//-----------------------//

static void virtual_irq_hw_enable (unsigned int irq) {
  unsigned long hw_flags;

  hw_save_flags_and_cli (&hw_flags);
  clear_bit (xm_root_domain -> events -> masked_events, irq);
  
  /*
  if (is_bit_set (global_pending_events, irq)) {
    if (hw_xpic[irq])
      hw_xpic[irq] -> enable(irq);
    clear_bit (global_pending_events, irq);
    }*/
  
  hw_restore_flags (hw_flags);
}

//--------------------//
// virtual_irq_hw_end //
//--------------------//

static void virtual_irq_hw_end (unsigned int irq) {
  virtual_irq_hw_enable (irq);
  if (irq != hwtimer.timer_event)
    hw_xpic[irq] -> end (irq);
}

//------------------------//
// virtual_irq_hw_disable //
//------------------------//

static void virtual_irq_hw_disable (unsigned int irq) {
  unsigned long hw_flags;
  hw_save_flags_and_cli (&hw_flags);
  set_bit (xm_root_domain -> events -> masked_events, irq);
  hw_restore_flags (hw_flags);
}

//-----------------------------//
// virtual_irq_hw_set_affinity //
//-----------------------------//

/*
static void virtual_irq_hw_set_affinity (unsigned irq, cpumask_t mask) {
  if(hw_xpic [irq] &&
     hw_xpic [irq] ->set_affinity) {
    hw_xpic [irq] -> set_affinity(irq, mask);
  }
}
*/

//------//
// vpic //
//------//

struct hw_interrupt_type vpic = {
  .typename = "XM PIC",
  .startup = virtual_irq_hw_startup,
  .shutdown =  virtual_irq_hw_shutdown,
  .enable = virtual_irq_hw_enable,
  .disable = virtual_irq_hw_disable,
  .ack =  virtual_irq_hw_ack,
  .end =  virtual_irq_hw_end
  //set_affinity: virtual_irq_hw_set_affinity
};
