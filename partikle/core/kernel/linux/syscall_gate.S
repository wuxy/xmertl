/*
 * $FILE: syscalls.S
 *
 * System calls, architecture-dependent part
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


#include <syscalls.h>
#include <arch/processor.h>
#include <config.h>
	
.text
.globl syscall_gate
.align 4
	
syscall_gate:
	cld
	PUSH_REGISTERS()
	cmpl $NR_SYSCALLS,%eax
        jae 1f
#ifdef CONFIG_PORT_DEVTRACE
    pushl %eax
    call syscall_pretrace
    popl %eax 
#endif
	movl syscall_table (, %eax,4), %eax
	testl %eax, %eax
	jz 1f
	call *%eax
#ifdef CONFIG_PORT_DEVTRACE
    pushl %eax
    pushl 0x1c(%esp)
    call syscall_posttrace
    addl $4, %esp
    popl %eax
#endif
	movl %eax, 0x18(%esp)
	jmp 2f
1:
	pushl 0x18(%esp)
	pushl $unknown_syscall
	call printf
	addl $0x8, %esp
	movl $-1, 0x18(%esp)
2:
	call do_signals
	POP_REGISTERS()
	/* going back ... */
	ret

unknown_syscall:	
	.asciz "System call (%d) unknown\n"
