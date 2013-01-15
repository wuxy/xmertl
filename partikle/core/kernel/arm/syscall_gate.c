/*
 * $FILE: syscalls.c
 *
 * Syscalls
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

#include <stdio.h>
#include <syscalls.h>

int syscall_gate (unsigned long a0, unsigned long a1, 
		  unsigned long a2, unsigned long a3, 
		  unsigned long a4, unsigned long syscallnr) {
  int ret;
  
  if (syscallnr >= NR_SYSCALLS) {
    printf ("System call (%d) unknown\n", syscallnr);
    return -1;
  }

  ret = ((int (*) (unsigned long, unsigned long, unsigned long, 
		   unsigned long, unsigned long))syscall_table [syscallnr]) 
    (a0, a1, a2, a3, a4);

  return ret;
}
