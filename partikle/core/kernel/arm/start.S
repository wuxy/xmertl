/*
 * $FILE: start.S
 *
 * Entry point of the kernel in the ARM architecture version
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

/*
 * This file must be kept as simple as possible
 */

#include <config.h>
#include <arch/processor.h>
	
.section .text.boot, "ax"
.globl start, _start
.align 4

start:
_start:
	/* set the cpu to SVC32 mode */
	mrs r0, cpsr
	bic r0, r0,#(PSR_MODE)
	orr r0, r0,#(PSR_SVC32_MODE)
	msr cpsr, r0
	
	b enter_partikle


		
	/* Flush caches, TLB, ... */
	/* This code could depend on the SUBARCH */
/*#if defined ARM926*/
	/* flush v4 I/D caches */
/*	mov r0, #0 */
/*	mcr p15, 0, r0, c7, c7, 0 */	/* flush v3/v4 cache */
/*	mcr p15, 0, r0, c8, c7, 0 */	/* flush v4 TLB */

	/* disable MMU stuff and caches */
/*	mrc p15, 0, r0, c1, c0, 0 */
/*	bic r0, r0, #0x00002300 */	/* clear bits 13, 9:8 (--V- --RS) */
/*	bic r0, r0, #0x00000087 */	/* clear bits 7, 2:0 (B--- -CAM) */
/*	orr r0, r0, #0x00000002 */	/* set bit 2 (A) Align */
/*	orr r0, r0, #0x00001000 */	/* set bit 12 (I) I-Cache */
/*	mcr p15, 0, r0, c1, c0, 0 */
/*#endif*/
