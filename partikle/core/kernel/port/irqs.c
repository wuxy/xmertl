/*
 * $FILE: irqs.c
 *
 * Independent part of interrupt handling
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
#include <stdio.h>
#include <processor.h>

// Definitions

irq_handler_t irq_handler_table [IRQ_NR];
trap_handler_t trap_handler_table [TRAP_NR];
hw_irq_ctrl_t hw_irq_ctrl [IRQ_NR];
volatile unsigned long irq_nesting_counter = 0;

//---------------------//
// default_irq_handler //
//---------------------//

int default_irq_handler (context_t *context) {
  printf ("Unexpected irq %d\n", context -> irqnr);
  return 0;
}

//-----------------//
// unexpected_trap //
//-----------------//

int unexpected_trap (context_t *context) {
  panic (context, "Unexpected/unhandled trap - TRAP: 0x%x ERROR CODE: 0x%x\n",
         context -> irqnr, context -> error_code);
  return -1;
}

//-----------------//
// init_irqs_traps //
//-----------------//

int init_irqs_traps (void) {
  int cont;

  for (cont = 0; cont < IRQ_NR; cont ++)
    irq_handler_table [cont] = default_irq_handler;
  
  for (cont = 0; cont < TRAP_NR; cont ++)
    trap_handler_table [cont] = unexpected_trap;

  init_arch_irqs ();
  
  return 0;
}

//-------------------------//
// install_irq_handler_sys //
//-------------------------//

asmlinkage irq_handler_t install_irq_handler_sys (int irq, 
						  irq_handler_t irq_handler) {
  irq_handler_t old_handler = irq_handler_table [irq];
  int flags;
  hw_save_flags_and_cli (flags);
  if (irq_handler)
    irq_handler_table [irq] = irq_handler;
  else
    irq_handler_table [irq] = default_irq_handler;
  hw_restore_flags (flags);
  return old_handler;
}

//--------------------------//
// install_trap_handler_sys //
//--------------------------//

asmlinkage 
trap_handler_t install_trap_handler_sys (int trap, 
					 trap_handler_t trap_handler) {
  trap_handler_t old_handler = trap_handler_table [trap];
  int flags;
  hw_save_flags_and_cli (flags);
  if (trap_handler)
    trap_handler_table [trap] = trap_handler;
  else
    trap_handler_table [trap] = unexpected_trap;
  hw_restore_flags (flags);
  return old_handler;
}

//--------------------//
// hw_disable_irq_sys //
//--------------------//

asmlinkage void hw_disable_irq_sys (int irq) {
  hw_disable_irq (irq);
}

//-------------------//
// hw_enable_irq_sys //
//-------------------//

asmlinkage void hw_enable_irq_sys (int irq) {
  hw_enable_irq (irq);
}

//----------------//
// hw_ack_irq_sys //
//----------------//

asmlinkage void hw_ack_irq_sys (int irq) {
  hw_ack_irq (irq);
}

//----------------//
// hw_end_irq_sys //
//----------------//

asmlinkage void hw_end_irq_sys (int irq) {
  hw_end_irq (irq);
}

//------------//
// hw_cli_sys //
//------------//

asmlinkage void hw_cli_sys (void) {
  hw_cli ();
}

//------------//
// hw_sti_sys //
//------------//

asmlinkage void hw_sti_sys (void) {
  hw_sti ();
}

//----------------------//
// hw_restore_flags_sys //
//----------------------//

asmlinkage void hw_restore_flags_sys (int flags) {
  hw_restore_flags (flags);
}

//-------------------//
// hw_save_flags_sys //
//-------------------//

asmlinkage void hw_save_flags_sys (int *flags) {
  hw_save_flags (*flags);
}

//---------------------------//
// hw_save_flags_and_cli_sys //
//---------------------------//

asmlinkage void hw_save_flags_and_cli_sys (int *flags) {
  hw_save_flags_and_cli (*flags);
}
