/*
 * $FILE: xmprintf.c
 *
 * XM's print
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

#include <arch/cpu.h>

#define MAX_PRINTKBUF 2000

static char in_printkbuf [MAX_PRINTKBUF];
static char *printkptr = &in_printkbuf[0];
volatile int xmprint = 0;

int fill_printk(char *str, int length) {
  unsigned long flags;
  int i = length;

  hw_save_flags_and_cli(&flags);
  
  if (i > MAX_PRINTKBUF - (printkptr - in_printkbuf) - 30) {
    i = MAX_PRINTKBUF - (printkptr - in_printkbuf) - 30;
  }
  
  if (i <= 0) {
    hw_restore_flags (flags);
    return 0;
  }

  memcpy (printkptr, str, i);
  printkptr += i;
  *printkptr = 0;
  
  xmprint = 1;

  hw_restore_flags (flags);
  return i;
}

static char out_printkbuf [MAX_PRINTKBUF];

void write_printkbuf (void) {
  unsigned long flags;

  hw_save_flags_and_cli (&flags);
  memcpy (out_printkbuf, in_printkbuf, printkptr - in_printkbuf + 1);

  printkptr = &in_printkbuf[0];
  *printkptr = 0;
  xmprint = 0;

  hw_restore_flags(flags);

  printk("%s", out_printkbuf);
}
