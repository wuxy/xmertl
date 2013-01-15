/*
 * $FILE: processor.c
 *
 * Processor arch-dependient functions
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

#include <config.h>
#include <irqs.h>
#include <arch/board.h>
#include <arch/io.h>

//-----------//
// setup_cpu //
//-----------//

int setup_cpu (void) {
  init_irqs_traps ();

  return 0;
}

//----------//
// iopl_sys //
//----------//

int iopl_sys (int level) {
  return 0;
}

//-------------//
// get_cpu_khz //
//-------------//

unsigned int get_cpu_khz (void) {
  unsigned int cm_osc;

  cm_osc = io_read (CM_OSC);

  return ((cm_osc & PLL_VDW) + 8) * 1000;
}
