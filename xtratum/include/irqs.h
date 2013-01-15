/*
 * $FILE: irqs.h
 *
 * IRQS related functions
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

#ifndef _XM_IRQS_H_
#define _XM_IRQS_H_

#include <linux_inc.h>
#include <arch/irqs.h>

#include <bitmap.h>

extern bitmap_t global_pending_events;

extern int hw_irq_takeover (void);
extern int hw_irq_giveup (void);

extern struct hw_interrupt_type vpic;
extern struct hw_interrupt_type *hw_xpic [NR_IRQS];
extern void linux_irq_handler (int irq, struct pt_regs *regs);
extern void sync_events (void);

#endif
