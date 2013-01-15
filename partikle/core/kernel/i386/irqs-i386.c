/*
 * $FILE: irqs.c
 *
 * Hardware-dependent part of irqs
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
#include <arch/processor.h>

//--------//
// do_irq //
//--------//
extern void scheduling (void);
void do_irq (context_t context) {
  irq_nesting_counter ++;
  if (hw_irq_ctrl [context.irqnr].ack)
    hw_ack_irq (context.irqnr);
  
  if (irq_handler_table [context.irqnr])
    (*irq_handler_table [context.irqnr]) (&context);
  else
    default_irq_handler (&context);

  if (hw_irq_ctrl [context.irqnr].end)
    hw_end_irq (context.irqnr);
  
  irq_nesting_counter --;
  if (irq_nesting_counter == SCHED_PENDING)
    scheduling ();   
}

//---------------//
// init_arch_irq //
//---------------//
extern void init_pic (unsigned char master_base, unsigned char slave_base);
int init_arch_irqs (void) {
  int irqnr, vector;
  // the address of the irq handlers
  void (**irq_addr)(void) = (void (**) (void))
    ((unsigned long)&_sirq_handlers);
  // the address of the trap handlers
  void (**trap_addr)(void) = (void (**) (void))
    ((unsigned long)&_strap_handlers);
  
  // In an i386 there are 16 irqs, 0..15 (besides of the acpi interrupt)
  for (irqnr = 0; irqnr < IRQ_NR; irqnr++) {
    vector = irqnr + FIRST_EXTERNAL_VECTOR;
    
    // Setting up all hw irq gates
    hw_set_irq_gate (vector, irq_addr [irqnr]);
  }
  
  /* 
   * XXX: the single difference between a trap_gate and an
   * interrupt_gate is that the IF flags is unmodified in the first
   * one whereas it is cleaned in the second one
   */
  
  hw_set_trap_gate (0, trap_addr [0]);
  hw_set_irq_gate (1, trap_addr [1]);
  hw_set_irq_gate (2, trap_addr [2]);
  hw_set_sys_gate (3, trap_addr [3]);
  hw_set_sys_gate (4, trap_addr [4]);
  hw_set_sys_gate (5, trap_addr [5]);
  hw_set_trap_gate (6, trap_addr [6]);
  hw_set_trap_gate (7, trap_addr [7]);
  hw_set_trap_gate (8, trap_addr [8]);
  hw_set_trap_gate (9, trap_addr [9]);
  hw_set_trap_gate (10, trap_addr [10]);
  hw_set_trap_gate (11, trap_addr [11]);
  hw_set_trap_gate (12, trap_addr [12]);
  hw_set_trap_gate (13, trap_addr [13]);
  hw_set_irq_gate (14, trap_addr [14]);
  hw_set_trap_gate (15, trap_addr [15]);
  hw_set_trap_gate (16, trap_addr [16]);
  hw_set_trap_gate (17, trap_addr [17]);
  hw_set_trap_gate (18, trap_addr [18]);
  hw_set_trap_gate (19, trap_addr [19]);

  init_pic (0x20, 0x28);

  return 0;
}
