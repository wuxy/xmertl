/* Sample initialization file */
#include "mem.h"

	.extern svcstack
	.extern maininit
.section .text
.arm
	.align 0
	.global LC0
	LC0:
		.word  svcstack + KSTACK // sizeof(svcstack)

	.code 32
	.align  0

	/* Start in supervisor mode, with interrupts initially disabled. */
start:
	// set stack for svc mode
	mov	r0, #PsrMsvc|PsrDirq|PsrDfiq
	ldr     r1, LC0
	bl	setr13

	bl	maininit
loop:
	b	loop

/*
 * ulong setr13(ulong mode, ulong *stack);
 */
	.align	2
	.global	setr13
	.type	setr13, %function
setr13:
	mrs	r2, CPSR
	bic	r3, r2, #PsrMask
	orr	r3, r3, r0
	msr	CPSR, r3
	
	mov	r0, r13
	mov	r13, r1
	
	msr	CPSR, r2
	mov	pc, lr
	.size	setr13, .-setr13

/*- Manage Exception Entry  */
/*- 1. Adjust and save LR_irq in IRQ stack  */
/*- 2. Save SPSR need to be saved for nested interrupt */
/*- 3. Save and r0 in IRQ stack  */
/* R14 = LR */
/*- 4. Write in the IVR to support Protect Mode	 */
/*- 4. No effect in Normal Mode  */
/*- 4. De-assert the NIRQ and clear the source in Protect Mode */
/*- 5. Enable Interrupt and Switch in Supervisor Mode */
/*- 6. Save scratch/used registers and LR in User Stack */
/*- 7. Branch to the routine pointed by the VIC-Vector-Address  */
/*- 8. Restore scratch/used registers and LR from User Stack*/
/*- 9. Disable Interrupt and switch back in IRQ mode */
/*- 10. Mark the End of Interrupt on the VIC */
/*- 11. Restore SPSR_irq and r0 from IRQ stack */
/*- 12. Restore SPSR_irq and r0 from IRQ stack */
/*- 13. Restore adjusted  LR_irq from IRQ stack directly in the PC */

	.global irq_handler
	.type	irq_handler, %function
irq_handler:
	sub	lr, lr, #4					// - 1.
	stmfd	sp!, {lr}

	mrs	r14, SPSR					// - 2.
	stmfd	sp!, {r14}

	stmfd	sp!, {r0}				// - 3.

	ldr	r14, =VICBASE				// - 4.
	ldr	r0 , [r14, #VICVOFF]
	/*str	  r14, [r14, #VICVOFF]*/

	msr	CPSR_c, #PsrMsvc			// - 5.

	/*stmfd	  sp!, { r1-r3, r12, r14}*/
	stmfd	sp!, { r1-r12, r14 }	// - 6.

	mov	r14, pc						// - 7.
	bx	r0

	/* ldmia	   sp!, { r1-r3, r12, r14} */
	ldmia	sp!, { r1-r12, r14 }	// - 8.

	msr	CPSR_c, #PsrDirq|PsrMirq	// - 9.

	
	/*ldr	r14, =VICBASE			// - 10.
	str	r14, [r14, #VICVOFF]*/
	

	ldmia	sp!, {r0}				// - 11.

	ldmia	sp!, {r14}				// - 12.
	msr	SPSR_cxsf, r14

	ldmia	sp!, {pc}^				// - 13.

.size   irq_handler, . - irq_handler

/*
 * void context_switch (pthread_t new, pthread_t *current);
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

.previous /* text */

.section .startup,"ax"
	.arm
	.code 32
	.align 0

	/* Setup vector table */
Vectors:
	ldr pc, _rst			/* reset - start */
	ldr	pc, _undf			/* undefined - _undf */
	ldr	pc, _swi			/* SWI - _swi */
	ldr	pc, _pabt			/* program abort - _pabt */
	ldr	pc, _dabt			/* data abort - _dabt */
	nop						/* reserved - cksum */
//	ldr	pc, [pc,#-0xFF0]	/* IRQ - read the VIC */
	ldr	pc, _irq_w
	ldr	pc, _fiq			/* FIQ - _fiq */

_rst:	.word start		/* Reset */
_undf:	.word __undf		/* undefined */
_swi:	.word __swi			/* SWI */
_pabt:	.word __pabt		/* program abort */
_dabt:	.word __dabt		/* data abort */
_irq_w:	.word __irq_w		/* IRQ wrapper */
_fiq:	.word __fiq			/* FIQ */

__undf:
	b	trap
__swi:
	b	trap
__pabt:
	b	trap
__dabt:
	b	trap
__fiq:
	b	trap
__irq_w:
	b	irq_handler
.size   Vectors, . - Vectors
.previous /* startup */
