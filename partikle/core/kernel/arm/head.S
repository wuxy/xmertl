/*
 * $FILE: head.S
 *
 * Start code
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
#include <arch/processor.h>

.globl enter_partikle, _exit
.text
.align 4
enter_partikle:
	mov fp, #0x00000000 /* Enabling backtrace */
	
	/* set up a valid stack */
	ldr sp, Lsstack
	
	/* Jump into C code */	
	bl setup_kernel

_exit:
	ldr r0, Lhalt_message
	bl printf

	/* Disabling interrupts */
	mrs r0, cpsr
	orr r0, r0, #(PSR_I)
	msr cpsr, r0
1:
	b 1b

Lhalt_message:
	.word halt_message

halt_message:
	.asciz "\nSystem halted.\n" ;

Lsstack:
	.word _Esstack

.data
_sstack:
	/* SVC stack */
	.fill (STACK_SIZE/4), 4, 0x0
_Esstack:
