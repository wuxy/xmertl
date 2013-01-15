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
#include <arch/board.h>
#include <arch/io.h>
#include <arch/processor.h>

//--------//
// do_irq //
//--------//
extern void set_stackptr(int, unsigned int *);
extern unsigned int abort_stack[], firq_stack[], irq_stack[], undef_stack[];

int do_irq (context_t *context) {
  int irqnr = pic_get_eirq ();
  
  //printf ("IRQ\n");

  if (hw_irq_ctrl [irqnr].ack)
    hw_ack_irq (irqnr);
  
  if (irq_handler_table [irqnr])
    (*irq_handler_table [irqnr]) (context);
  else
    default_irq_handler (context);
  
  if (hw_irq_ctrl [irqnr].end)
    hw_end_irq (irqnr);
  
  return 0;
}

//-----------------//
// fix_excpt_table //
//-----------------//

/* Needed to fix the exception table when it doesn't exist
   (QEMU case)
 */
static unsigned long excpt_fixed_table [] = {
  0xea000006, 0xe59ff018, 0xe59ff018, 0xe59ff018, 
  0xe59ff018, 0xe59ff018, 0xe59ff018, 0xe59ff018
};

extern void fix_excpt_table (void) {
  int n;
  extern unsigned long exception_vectors [];
  unsigned long *vector = 0;
  
  for (n = 0; n < 8; n++) {
    vector [n] = excpt_fixed_table [n];
    vector [n + 8] = exception_vectors[n];
  }
}


//----------------------//
// install_excp_handler //
//----------------------//

/*static int install_excp_handler (int excpt, unsigned long *addr) {
  unsigned int *vectors = (unsigned int *)0;
*/
/* Checks whether the first instruction executed by the
   exception is a LDR */
/*if ((vectors [excpt] & 0x4500000) == 0x4100000) {
  
return 0;
}*/
  /*
  return -1;
}
  */
//---------------//
// init_arch_irq //
//---------------//

extern void set_stackptr(int, unsigned int *);
extern unsigned int abort_stack[], firq_stack[], irq_stack[], undef_stack[];
//extern unsigned int base_trap_vectors[];

extern void init_pic (void);
int init_arch_irqs (void) {
  set_stackptr (PSR_ABT32_MODE, abort_stack);
  // BUG in QEMU with PSR_FIQ32_MODE
  //set_stackptr (PSR_FIQ32_MODE, firq_stack);
  set_stackptr (PSR_IRQ32_MODE, irq_stack);
  set_stackptr (PSR_UND32_MODE, undef_stack);

  fix_excpt_table ();
  
  trap_handler_table [EXCPT_IRQ] = do_irq;

  init_pic ();

  return 0;
}
