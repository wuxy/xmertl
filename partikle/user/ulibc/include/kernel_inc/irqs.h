/*
 * $FILE: irqs.h
 *
 * Arch-independent part of the interrupts and traps
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

#ifndef _IRQS_H_
#define _IRQS_H_

#ifndef _KERNEL_
#include <kernel_inc/arch/irqs.h>
#else
#include <arch/irqs.h>
#endif

typedef int (*irq_handler_t)(context_t *);
typedef int (*trap_handler_t)(context_t *);

#ifdef _KERNEL_
#include <linkage.h>

#define OUTSIDE_IRQS 0x7fffffff
#define SCHED_PENDING 0x80000000

extern volatile unsigned long irq_nesting_counter;
extern irq_handler_t irq_handler_table [IRQ_NR];
extern trap_handler_t trap_handler_table [TRAP_NR];
extern int default_irq_handler (context_t *);

extern int init_irqs_traps (void);
extern asmlinkage irq_handler_t install_irq_handler_sys (int, irq_handler_t);
extern asmlinkage trap_handler_t install_trap_handler_sys (int, 
							   trap_handler_t);

extern int init_arch_irqs (void);

// Control over each interrupt
typedef struct hw_irq_ctrl_struct {
  void (*enable) (unsigned int irq);
  void (*disable) (unsigned int irq);
  void (*ack) (unsigned int irq);
  void (*end) (unsigned int irq);
} hw_irq_ctrl_t;

extern hw_irq_ctrl_t hw_irq_ctrl [IRQ_NR];

static inline void hw_disable_irq (int irq) {
  hw_irq_ctrl [irq].disable (irq);
}

static inline void hw_enable_irq (int irq) {
  hw_irq_ctrl [irq].enable (irq);
}

static inline void hw_ack_irq (int irq) {
  hw_irq_ctrl [irq].ack (irq);
}

static inline void hw_end_irq (int irq) {
  hw_irq_ctrl [irq].end (irq);
}

extern asmlinkage void hw_disable_irq_sys (int irq);
extern asmlinkage void hw_enable_irq_sys (int irq);
extern asmlinkage void hw_ack_irq_sys (int irq);
extern asmlinkage void hw_end_irq_sys (int irq);
extern asmlinkage void hw_cli_sys (void);
extern asmlinkage void hw_sti_sys (void);
extern asmlinkage void hw_restore_flags_sys (int flags);
extern asmlinkage void hw_save_flags_sys (int *flags);
extern asmlinkage void hw_save_flags_and_cli_sys (int *flags);
#endif
#endif
