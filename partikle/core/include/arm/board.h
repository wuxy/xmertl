/*
 * $FILE: board.h
 *
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

#ifndef _ARCH_BOARD_H_
#define _ARCH_BOARD_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <config.h>


#ifdef CONFIG_BOARD_INTEGRATORCP

#define CM_ID 0x10000000
#define CM_PROC 0x10000004
#define CM_OSC 0x10000008
#define CM_CTRL 0x1000000C
#define CM_STAT 0x10000010
#define CM_SDRAM 0x10000020
#define CM_INIT 0x10000024

#define CM_IRQ_STAT 0x10000040
#define CM_IRQ_RSTAT 0x10000044
#define CM_IRQ_ENSET 0x10000048
#define CM_IRQ_ENCLR 0x1000004c
#define CM_SOFT_INTSET 0x10000050
#define CM_SOFT_INTCLR 0x10000054
#define CM_FIQ_STAT 0x10000060
#define CM_FIQ_RSTAT 0x10000064
#define CM_FIQ_ENSET  0x10000068
#define CM_FIQ_ENCLR 0x1000006c

#define PIT_BASE_ADDR 0x13000000

#define PIC_BASE_ADDR 0x14000000

/* Related to CM_SDRAM */
#define NBANKS 0xF0000
#define MEMSIZE 0x1C

#define CM_SDRAM_MEMSIZE2MEM(x) (0x1000000 << x)

/* Related to CM_OSC */
#define PLL_VDW 0xFF

#define ICP_TIMER0_IRQ 5
#define ICP_TIMER1_IRQ 6
#define ICP_TIMER2_IRQ 7

#endif

#endif
