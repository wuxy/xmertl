/*
 * $FILE: start.S
 *
 * Entry point of the kernel in the XtratuM 86 architecture version
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
	
.text

.globl start, _start, _exit
.align 4

start:
_start:
        cld			;
	movl 4(%esp), %eax	;
	movl %eax, event_handling ;
	
/*        movl 8(%esp), %ebx      ;
        movl %ebx, xm_heap_addr ;
        movl 4(%esp), %ebx      ;
        movl %ebx, xm_heap_size ;
        movl (%esp), %ebx       ;
        movl %ebx, event_handling ;*/
	/* Reseting EFLAGS */
/*	pushl $0		;
	popf*/
	
	/* Jumping to the first C function in the kernel */
        call setup_kernel       ;

_exit:
        pushl $halt_message     ;
        call printf             ;
        movl $0, %eax           ;
        int $0x82               ;
        
/*
        pushl $0                 ;
        call xm_exit_domain	 ;
*/
halt_message:
        .asciz "\nSystem halted.\n" ;
