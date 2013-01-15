/*
 * $FILE: irqs.h
 *
 * Arch-dependent part of the exceptions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mmasmano@ai2.upv.es>
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

#ifndef _ARCH_EXCEPTIONS_H_
#define _ARCH_EXCEPTIONS_H_

#include <config.h>

#define TRAP_NR 8

#define IRQ_NR 32 /* Board dependent */

#define EXCPT_RESET 0
#define EXCPT_UNDEF_INSTR 1 /* undefined instruction */
#define EXCPT_SWI 2 /* software interrupt */
#define EXCPT_PREFETCH_ABORT 3 /* instruction page fault */
#define EXCPT_DATA_ABORT 4 /* data page fault */
#define EXCPT_UNUSED 5
#define EXCPT_IRQ 6 /* Interrupt Request */
#define EXCPT_FIRQ 7 /* Fast Interrupt Request */

#define EXCPT_STATE_EXCPTNO 64
#define EXCPT_STATE_INTNO 68
#define EXCPT_STATE_PC 80

#ifndef __ASSEMBLY__

extern unsigned int pic_get_eirq (void);

typedef struct context_struct {
  unsigned long spsr;
  unsigned long r0;
  unsigned long r1;
  unsigned long r2;
  unsigned long r3;
  unsigned long r4;
  unsigned long r5;
  unsigned long r6;
  unsigned long r7;
  unsigned long r8;
  unsigned long r9;
  unsigned long r10;
  unsigned long r11;
  unsigned long r12;
  unsigned long usr_sp;
  unsigned long usr_lr;
  unsigned long error_code;
  unsigned long irqnr;
  unsigned long svc_sp;
  unsigned long svc_lr;
  unsigned long pc;
} context_t;

#endif
#endif
