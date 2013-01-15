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
}

void panic (context_t *regs, char *fmt, ...) {
  va_list vl;
  
  printf ("\n\nSystem PANIC:\n");
  va_start (vl, fmt);
  vprintf (fmt, vl);
  va_end (vl);

  if (regs)
    dump_state (regs);

  _exit(-1);
}
