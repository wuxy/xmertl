/*
 * $FILE: head.S
 *
 * Start code after enabling paging
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
#include <arch/multiboot.h>
#include <arch/paging.h>
#include <arch/processor.h>
	
.globl enter_partikle, _exit, pgd, pg0, pg_pgt, gdt, idt, _sstack, __halt_beeping
.text
.align 4
enter_partikle:
	/* setting up a valid stack */
	movl $(_sstack + STACK_SIZE), %esp

	/* reset EFLAGS */
	pushl $0
	popf
	
	/* setting up a valid gdt */
	lgdt gdt_desc

	/* loading descriptors from our new gdt */
	ljmp $(KERNEL_CS),$1f
	
1:	movl $(KERNEL_DS), %eax
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %ss

	/* loading a valid idt */
	lidt idt_desc

	/* checking the processor type */
	pushfl                  ; // is a 386?
	popl %eax               ; // get EFLAGS
	movl %eax,%ecx          ; // save original EFLAGS
	xorl $0x40000,%eax      ; // set AC bit in EFLAGS
	pushl %eax              ; // copy to EFLAGS
	popfl                   ; // set EFLAGS
	pushfl                  ; // get new EFLAGS
	popl %eax               ; // put it in eax
	xorl %ecx,%eax          ; // change in flags
	andl $0x40000,%eax      ; // check the state of the AC bit
	je is386

	movl %ecx,%eax
	xorl $0x200000,%eax     ; // check ID flag
	pushl %eax
	popfl                   ; // the 486 cannot change this bit
	pushfl                  ;
	popl %eax
	xorl %ecx,%eax
	push %ecx
	popfl
	andl $0x200000,%eax
	je is486

	xorl %eax,%eax          ; // CPUID with 0
	cpuid

	orl %eax,%eax           ; // It could be a 486
	je is486

	movl $1,%eax            ; // Reading CPU features
	cpuid
	movl %edx,cpu_fflags

	/* Jumping to the first C function in the kernel */
	call setup_kernel
_exit:
	pushl $halt_message
	call printf
        
	cli	
1:	hlt
	jmp 1b

is386:
is486:
	pushl $processor_not_supported
	call printf
1:	hlt
	jmp 1b

   /* the multiboot magic number is not valid, the machine is halted
      with a loud beep */
__halt_beeping:
	mov    $0xb6,%al
	mov    $0x43,%edx
	out    %al,(%dx)
	xor    %eax,%eax
	mov    $0x42,%edx
	out    %al,(%dx)
	mov    $0x6,%al
	out    %al,(%dx)
	mov    $0x61,%edx
	in     (%dx),%al
	or     $0x3,%al
	out    %al,(%dx)
1:	hlt
	jmp 1b

.data
.align 4096
_sstack:
	.fill (STACK_SIZE/4), 4, 0

#ifdef CONFIG_X86_PAGING
.align 4096

/* The PaRTiKle's Page Global Directory */
pgd:
	.fill 1024, 4, 0

/* Our first free page for mapping PaRTiKle */
pg0:
	.fill 1024, 4, 0

/* We use this page to map the pgt's */
pg_pgt:
	.fill 1024, 4, 0

#endif
	
.align 4

/* The PaRTiKle's GDT */
gdt:
	.quad 0x0000000000000000 /* NULL descriptor */
	.quad 0x00cf9a000000ffff /* 0x8 PaRTiKle code segment */
	.quad 0x00cf92000000ffff /* 0x10 PaRTiKle data segment */
	.quad 0x00cb9a000000ffff /* 0x18 app. code segment */
	.quad 0x00cb92000000ffff /* 0x20 app. data segment */
	.quad 0x0000000000000000 /* 0x28 TSS entry, if required */
	.fill GDT_ENTRIES-6,8,0

/* The IDT table */
idt:
	.fill IDT_ENTRIES,8,0

	.word 0
gdt_desc:
	.word GDT_ENTRIES*8-1
	.long gdt

	.word 0
idt_desc:
	.word IDT_ENTRIES*8-1
	.long idt

halt_message:
	.asciz "\nSystem halted.\n" ;

processor_not_supported:
	.asciz "CPU model not supported\nSystem halted."
