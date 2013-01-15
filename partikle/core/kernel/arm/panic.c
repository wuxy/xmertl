/*
 * $FILE: panic.c
 *
 * PANIC !!!!
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

#include <arch/irqs.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern void _exit (int);

/* dump_state dumps the CPU registers */
void dump_state (context_t *regs) {

  printf ("\nCurrent CPU state:\n");
  printf("Dump of trap_state at %p (trapno: %d):\n", regs, regs->irqnr);
  printf("R0  0x%x R1  0x%x R2  0x%x R3  0x%x\n",
	 regs->r0, regs->r1, regs->r2, regs->r3);
  printf("R4  0x%x R5  0x%x R6  0x%x R7  0x%x\n",
	 regs->r4, regs->r5, regs->r6, regs->r7);
  printf("R8  0x%x R9  0x%x R10 0x%x R11 0x%x R12 0x%x\n",
	 regs->r8, regs->r9, regs->r10, regs->r11, regs->r12);
  printf("USP 0x%x ULR 0x%x KSP 0x%x KLR 0x%x\n",
	 regs->usr_sp, regs->usr_lr, regs->svc_sp, regs->svc_lr);
  printf("PC  0x%x SPSR 0x%x\n",
	 regs->pc, regs->spsr);
  /*printf("FAR 0x%x FSR 0x%x\n",
    get_far(), get_fsr()); */
}

void panic (context_t *regs, char *fmt, ...) {
  va_list vl;
  
  printf ("\n\nSystem PANIC:\n");
  va_start (vl, fmt);
  vprintf (fmt, vl);
  va_end (vl);

  if (regs)
    dump_state (regs);
  
  _exit (-1);
}
