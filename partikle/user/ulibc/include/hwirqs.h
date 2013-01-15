/*
 * $FILE: hwirqs.h
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

#ifndef _ULIBC_HWIRQS_H_
#define _ULIBC_HWIRQS_H_

#include <kernel_inc/irqs.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

/* 
 * Unlike signals and other call-back functions, irq handlers and trap
 * handlers are executed with supervisor permissions
 */

extern irq_handler_t install_irq_handler (int, irq_handler_t);
extern trap_handler_t install_trap_handler (int, trap_handler_t);

extern void hw_disable_irq (int);
extern void hw_enable_irq (int);
extern void hw_ack_irq (int);
extern void hw_end_irq (int);
extern void hw_cli (void);
extern void hw_sti (void);
extern void hw_restore_flags (int flags);
extern void hw_save_flags (int *flags);
extern void hw_save_flags_and_cli (int *flags);

END_DECLS

#endif
