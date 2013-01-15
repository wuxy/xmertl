/*
 * $FILE: entry.S
 *
 * Interrupt's code
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

#include <arch/irqs.h>
#include <arch/processor.h>

#define IRQNR_SP 40
#define ERROR_CODE_SP 44

#define HW_SAVE_ALL \
  cld				; \
  pushl %es			; \
  pushl %ds			; \
  pushl %eax			; \
  pushl %ebp			; \
  pushl %edi			; \
  pushl %esi			; \
  pushl %edx			; \
  pushl %ecx			; \
  pushl %ebx			; \
  movl $(KERNEL_DS),%edx        ; \
  movl %edx,%ds			; \
  movl %edx,%es                 ; \
  movl %edx,%ss

#define HW_RESTORE_ALL  \
  popl %ebx			; \
  popl %ecx			; \
  popl %edx			; \
  popl %esi			; \
  popl %edi			; \
  popl %ebp			; \
  popl %eax			; \
  popl %ds			; \
  popl %es			; \
  addl $8,%esp			; \
  iret

#define IRQ_ADDR_TABLE_START \
  .section .rodata.irq_handlers,"a" ; \
  .globl _sirq_handlers		    ;  \
  .align 4			    ; \
  _sirq_handlers:	            ; \
  .previous

#define IRQ_ADDR_TABLE_END \
  .section .rodata.irq_handlers,"a" ; \
  .globl _eirq_handlers		    ; \
  .align 4			    ; \
  _eirq_handlers:	            ; \
  .previous			    ; \


/* BUILD_IRQ stores in the stack, the triggered irq 
   and 0 (no error code) */
#define BUILD_IRQ(irq) \
  .section .rodata.irq_handlers,"a" ; \
  .align 4			; \
  .long irq_handler_##irq	; \
  .text				; \
  .align 4			; \
  irq_handler_##irq:		; \
  pushl $0			; /* The error code (no error code) */ \
  pushl $irq			; /* The irq */ \
  jmp common_irq_body

#define BUILD_COMMON_IRQ_BODY \
  .text				; \
  .align 4			; \
  common_irq_body:		; \
  HW_SAVE_ALL			; \
  call do_irq			; \
  call do_signals		; \
  HW_RESTORE_ALL
	
#define TRAP_ADDR_TABLE_START \
  .section .rodata.trap_handlers,"a" ; \
  .globl _strap_handlers	; \
  .align 4			; \
  _strap_handlers:	 	; \
  .previous

#define TRAP_ADDR_TABLE_END \
  .section .rodata.trap_handlers,"a" ; \
  .globl _etrap_handlers	; \
  .align 4			; \
  _etrap_handlers:		; \
  .previous

#define BUILD_TRAP_ERRCODE(trapnr) \
  .section .rodata.trap_handlers,"a" ; \
  .align 4			; \
  .long trap_handler_##trapnr ; \
  .text				; \
  .align 4			; \
  trap_handler_##trapnr:	; \
  pushl $trapnr			; /* error_code has already been filled */ \
  jmp common_trap_body

#define BUILD_TRAP_NOERRCODE(trapnr) \
  .section .rodata.trap_handlers,"a" ; \
  .align 4			; \
  .long trap_handler_##trapnr ; \
  .text				; \
  .align 4			; \
  trap_handler_##trapnr:	; \
  pushl $0			; /* the error_code (no error code) */ \
  pushl $trapnr			;  /* the trap */ \
  jmp common_trap_body

#define BUILD_COMMON_TRAP_BODY \
  .text				; \
  .align 4			; \
  common_trap_body:		; \
  HW_SAVE_ALL			; \
  mov IRQNR_SP(%esp), %eax	; \
  movl trap_handler_table(,%eax,4), %eax ; \
  orl %eax,%eax			; \
  jz 1f				; \
  pushl %esp			; \
  call *%eax			; \
  addl $4,%esp			; \
  orl %eax,%eax			; \
  jnz 1f			; \
  call do_signals		; \
  HW_RESTORE_ALL		; \
  1: pushl %esp			; \
  call unexpected_trap	        ; \
  addl $4,%esp			; \
  2: hlt			; \
  jmp 2b

/* Interrupt routines */

IRQ_ADDR_TABLE_START
BUILD_IRQ(0x00)
BUILD_IRQ(0x01)
BUILD_IRQ(0x02)
BUILD_IRQ(0x03)
BUILD_IRQ(0x04) 
BUILD_IRQ(0x05)
BUILD_IRQ(0x06) 
BUILD_IRQ(0x07)
BUILD_IRQ(0x08)
BUILD_IRQ(0x09) 
BUILD_IRQ(0x0a)
BUILD_IRQ(0x0b)
BUILD_IRQ(0x0c)
BUILD_IRQ(0x0d)
BUILD_IRQ(0x0e)
BUILD_IRQ(0x0f)
#ifdef LOCAL_APIC
BUILD_IRQ(0x30)
#endif
IRQ_ADDR_TABLE_END

/* And, of course, next table is the trap one */

TRAP_ADDR_TABLE_START
BUILD_TRAP_NOERRCODE(0x0)
BUILD_TRAP_NOERRCODE(0x1)
BUILD_TRAP_NOERRCODE(0x2)
BUILD_TRAP_NOERRCODE(0x3)
BUILD_TRAP_NOERRCODE(0x4) 
BUILD_TRAP_NOERRCODE(0x5)
BUILD_TRAP_NOERRCODE(0x6) 
BUILD_TRAP_NOERRCODE(0x7)
BUILD_TRAP_ERRCODE(0x8)
BUILD_TRAP_NOERRCODE(0x9)
BUILD_TRAP_ERRCODE(0xa) 
BUILD_TRAP_ERRCODE(0xb)
BUILD_TRAP_ERRCODE(0xc) 
BUILD_TRAP_ERRCODE(0xd)
BUILD_TRAP_ERRCODE(0xe)
BUILD_TRAP_NOERRCODE(0xf)
BUILD_TRAP_NOERRCODE(0x10) 
BUILD_TRAP_ERRCODE(0x11)
BUILD_TRAP_NOERRCODE(0x12) 
BUILD_TRAP_NOERRCODE(0x13)
BUILD_TRAP_ERRCODE(0x14) 
BUILD_TRAP_ERRCODE(0x15)
BUILD_TRAP_ERRCODE(0x16)	
BUILD_TRAP_ERRCODE(0x17)
BUILD_TRAP_ERRCODE(0x18) 
BUILD_TRAP_ERRCODE(0x19)
BUILD_TRAP_ERRCODE(0x1a) 
BUILD_TRAP_ERRCODE(0x1b)
BUILD_TRAP_ERRCODE(0x1c) 
BUILD_TRAP_ERRCODE(0x1d)
BUILD_TRAP_ERRCODE(0x1e) 
BUILD_TRAP_ERRCODE(0x1f)
TRAP_ADDR_TABLE_END

BUILD_COMMON_TRAP_BODY
BUILD_COMMON_IRQ_BODY
