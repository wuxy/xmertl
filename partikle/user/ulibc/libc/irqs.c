/*
 * $FILE: irqs.c
 *
 * HW IRQs management functions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *
  * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <syscalls.h>
#include <hwirqs.h>

//---------------------//
// install_irq_handler //
//---------------------//

_syscall2(irq_handler_t, install_irq_handler, int, irq, irq_handler_t, hdl);

//----------------------//
// install_trap_handler //
//----------------------//

_syscall2(trap_handler_t, install_trap_handler, int, 
	  trap, trap_handler_t, hdl);

//----------------//
// hw_disable_irq //
//----------------//

void hw_disable_irq (int irq) {
  long __res;
  __syscall1 (irq, hw_disable_irq_nr, __res);
}

//---------------//
// hw_enable_irq //
//---------------//

void hw_enable_irq (int irq) {
  long __res;
  __syscall1 (irq, hw_enable_irq_nr, __res);
}

//------------//
// hw_ack_irq //
//------------//

void hw_ack_irq (int irq) {
  long __res;
  __syscall1 (irq, hw_ack_irq_nr, __res);
}

//------------//
// hw_end_irq //
//------------//

void hw_end_irq (int irq) {
  long __res;
  __syscall1 (irq, hw_end_irq_nr, __res);
}

//--------//
// hw_cli //
//--------//

void hw_cli (void) {
  long __res;
  __syscall0 (hw_cli_nr, __res);
}

//--------//
// hw_sti //
//--------//

void hw_sti (void) {
  long __res;
  __syscall0 (hw_sti_nr, __res);
}

//------------------//
// hw_restore_flags //
//------------------//

void hw_restore_flags (int flags) {
  long __res;
  __syscall1 (flags, hw_restore_flags_nr, __res);
}

//---------------//
// hw_save_flags //
//---------------//

void hw_save_flags (int *flags) {
  long __res;
  __syscall1 (flags, hw_save_flags_nr, __res);
}

//-----------------------//
// hw_save_flags_and_cli //
//-----------------------//

void hw_save_flags_and_cli (int *flags) {
  long __res;
  __syscall1 (flags, hw_save_flags_and_cli_nr, __res);
}
