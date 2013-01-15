/*
 * $FILE: processor.c
 *
 * Processor-related functions
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

#include <hwtimers.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <irqs.h>
#include <arch/xm_syscalls.h>
#include <arch/processor.h>

//-------------//
// get_cpu_khz //
//-------------//

hwtime_t get_cpu_khz (void) {
  return (hwtime_t) xm_get_cpu_khz();
}

//----------//
// iopl_sys //
//----------//

int iopl_sys (int val) {
  return 0;
}

//-----------//
// setup_cpu //
//-----------//

int setup_cpu (void) {
  init_irqs_traps ();
  return 0;
}
