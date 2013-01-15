/*
 * $FILE: entry.S
 *
 * Interrupts
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
#include <arch/irqs.h>
#include <arch/processor.h>
		
#define EXCEPTION(n, name) \
	.align 0		; \
.global name			; \
name##:				; \
	stmdb sp, {r0-r4}	; \
	mov r4, #(n)		; \
	b common_excp		; \
	mov r0, r0

.data
.align 4
		
.global exception_vectors
exception_vectors:
	.word 0 /* excpt_reset */
	.word excpt_undef_instr
	.word excpt_swi
	.word excpt_prefetch_abort
	.word excpt_data_abort
	.word excpt_unused
	.word excpt_irq
	.word excpt_firq

.text
/*EXCEPTION(EXCPT_RESET, excpt_reset)*/ /* 0x0 */
EXCEPTION(EXCPT_UNDEF_INSTR, excpt_undef_instr) /* 0x0 */
EXCEPTION(EXCPT_SWI, excpt_swi) /* 0x4 */
EXCEPTION(EXCPT_PREFETCH_ABORT, excpt_prefetch_abort) /* 0x8 */
EXCEPTION(EXCPT_DATA_ABORT, excpt_data_abort) /* 0xc */
EXCEPTION(EXCPT_UNUSED, excpt_unused) /* 0x10 */
EXCEPTION(EXCPT_IRQ, excpt_irq) /* 0x14 */
EXCEPTION(EXCPT_FIRQ, excpt_firq) /* 0x18 */

/* common routine for all exceptions  */
.align 0
common_excp:
	mov r0, lr /* save r14 */
	mov r1, sp /* save sp */
	mrs r3, spsr_all /* save spsr */
	mrs r2, cpsr_all /* save cpsr */
	bic r2, r2, #(PSR_MODE) /* Fix for SVC mode to SVC32 */
	orr r2, r2, #(PSR_SVC32_MODE)
	msr cpsr_all, r2 /* Go to SVC32 */
	mov r2, sp
	str r0, [sp, #-4]! /* Push return address */
	str lr, [sp, #-4]! /* Push SVC lr */
	str r2, [sp, #-4]! /* Push SVC sp */
        str r4, [sp, #-4]! /* Push interrupt request num slot */
        str r4, [sp, #-4]! /* Push trap number */
	
        msr spsr_all, r3 /* Restore correct spsr */
        ldmdb r1, {r0-r4} /* Restore 5 regs from xxx mode */
        sub sp, sp, #(4*15) /* Adjust the stack pointer */
        stmia sp, {r0-r12} /* Push the user mode registers */
        add r0, sp, #(4*13) /* Adjust the stack pointer */
        stmia r0, {r13-r14}^ /* Push the user mode registers */
        mov r0, r0 /* NOP for previous instruction */
        mrs r0, spsr_all /* Put the SPSR on the stack */
        str r0, [sp, #-4]!
	
	/* Index into the exception table to get the proper handler */
	ldr r3, Lhandlers
	ldr r4, [sp, #(EXCPT_STATE_EXCPTNO)] /* reload the excptno*/
	ldr r3, [r3, r4, asl #2]

	/* Call the trap handler, passing the stack pointer as arg0 */
	mov r0, sp
	mov lr, pc
	mov pc, r3
	
	/* Back from trap handler, If the handler function returned
	   zero (success), then resume execution as if the trap never happend,
	   otherwise, just panic */
	cmp r0, #0
	beq justreturn
	mov r0, sp
	bl panic
justreturn:
	ldr r0, [sp], #0x4 /* Get the SPSR from stack */
	msr spsr_all, r0 /* restore SPSR */
	ldmia sp, {r0-r14}^ /* restore registers (usr mode) */
	mov r0, r0
	add sp, sp, #(4*17) /* Adjust the stack pointer */
	ldmia sp, {sp, lr, pc}^
	mov r0, r0

Lhandlers:
	.word trap_handler_table

/*
 * Each mode the processor enters has its own stack pointer.
 * To set the stack pointer for a particular mode we must switch
 * to thar mode update the banked r13 and the switch back.
 * r0 = CPU mode
 * r1 = stackptr
 */
.global set_stackptr
set_stackptr:
	mrs r3, cpsr_all /* Switch to the appropiate mode*/
	bic r2, r3, #(PSR_MODE)
	orr r2, r2, r0
	msr cpsr_all, r2
	mov sp, r1 /* Set the stack pointer */
	msr cpsr_all, r3 /* Restore the old mode*/
	mov pc, lr /* return */

/* System stacks for each of the modes we are interested in. */
.bss
.align 12
.space 128
.global abort_stack, irq_stack, firq_stack, undef_stack, overflow_stack
	.space 128
abort_stack:
	.space 128
irq_stack:
	.space 128
firq_stack:
	.space 128
undef_stack:
	.space 4096

.text
/******************/
/* Context switch */
/******************/
/*
extern void context_switch (pthread_t new_thread, pthread_t *current_thread);
	                                 r0                     r1
*/
.global context_switch
context_switch:
	stmdb sp!, {r0 - r10, fp, lr}
	mrs ip, cpsr
	str ip, [sp, #-4]!
	ldr r2, [r1]
	str sp, [r2]
	ldr sp, [r0]
	str r0, [r1]                @ set current task to new task
	ldr ip, [sp], #4
	msr spsr, ip
	ldmia sp!, {r0 - r10, fp, pc}^

