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
  printf ("EIP: 0x%x:[<0x%x>]", (unsigned int) 0xffff & regs->xcs, 
	     (unsigned int) regs->eip);
  
/* if (regs->xcs & 3)
    printf (" ESP: 0x%x:0x%x", (unsigned int) 0xffff & regs->xss,
	       (unsigned int) regs->esp);
  */
  printf (" EFLAGS: 0x%x  \n", (unsigned int) regs->eflags);
  printf ("EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n",
	     (unsigned int) regs->eax, (unsigned int) regs->ebx,
	     (unsigned int) regs->ecx, (unsigned int) regs->edx);
  printf ("ESI: 0x%x EDI: 0x%x EBP: 0x%x",
	     (unsigned int) regs->esi, (unsigned int) regs->edi, 
	     (unsigned int) regs->ebp);
  printf (" DS: 0x%x ES: 0x%x\n",
	     (unsigned int) 0xffff & regs->xds,
	     (unsigned int) 0xffff & regs->xes);
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
