/*
 * $FILE: pic.h
 *
 * The PC's PIC
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

#ifndef _PIC_H_
#define _PIC_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

// Without the use of the IO-APIC, the pic controlls interrupts
// from 0 to 15 (16 irqs)
#define PIC_IRQS 16

extern void init_pic (unsigned char master_base, unsigned char slave_base);

#endif
