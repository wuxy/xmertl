/*
 * $FILE: irqs.h
 *
 * IRQS related functions
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

#ifndef _XM_ARCH_IRQS_H_
#define _XM_ARCH_IRQS_H_

#define MAX_TRAPS 20

#ifndef UXMLIB

#include <linux_inc.h>

#include <syscalls.h>

extern struct desc_struct *real_idt_table;

static inline void *hw_get_idt_table_addr(void) {
  struct desc_struct idt;
  __asm__ __volatile__ ("sidt %0\n\t" : "=m" (idt):); 
  return ((void *)
	  (((idt.a & 0xffff0000) >> 16)|((idt.b & 0xffff)) << 16));
}

/* Directly taken from arch/i386/kernel/traps.c */
#define hw_set_gate(gate_addr,type,dpl,addr)  \
do { \
  int __d0, __d1; \
  __asm__ __volatile__ ("movw %%dx,%%ax\n\t" \
       "movw %4,%%dx\n\t" \
       "movl %%eax,%0\n\t" \
       "movl %%edx,%1" \
       :"=m" (*((long *) (gate_addr))), \
        "=m" (*(1+(long *) (gate_addr))), "=&a" (__d0), "=&d" (__d1) \
       :"i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
        "3" ((char *) (addr)),"2" (__KERNEL_CS << 16)); \
} while (0)

/* Partially taken from arch/i386/kernel/traps.c */
#define hw_get_gate_addr(e) \
  ((void *)((real_idt_table[e].b & 0xffff0000)|(real_idt_table[e].a & 0xffff)))

/* Partially taken from arch/i386/kernel/traps.c */
#define hw_set_irq_gate(vector,addr) \
  hw_set_gate(real_idt_table+vector,14,0,addr)

/* Partially taken from arch/i386/kernel/traps.c */
#define hw_set_trap_gate(vector,addr) \
  hw_set_gate(real_idt_table+vector,15,0,addr)

/* Partially taken from arch/i386/kernel/traps.c */
#define hw_set_sys_gate(vector,addr) \
  hw_set_gate(real_idt_table+vector,15,3,addr)

// IDT routines

#define HW_SAVE_ALL \
        "pushl %es\n\t" \
        "pushl %ds\n\t" \
        "pushl %eax\n\t" \
        "pushl %ebp\n\t" \
        "pushl %edi\n\t" \
        "pushl %esi\n\t" \
        "pushl %edx\n\t" \
        "pushl %ecx\n\t" \
        "pushl %ebx\n\t" \
        "movl $" STR(__USER_DS) ",%edx\n\t" \
        "movl %edx,%ds\n\t" \
        "movl %edx,%es\n\t"

#define HW_RESTORE_ALL  \
        "popl %ebx\n\t" \
        "popl %ecx\n\t" \
        "popl %edx\n\t" \
        "popl %esi\n\t" \
        "popl %edi\n\t" \
        "popl %ebp\n\t" \
        "popl %eax\n\t" \
        "popl %ds\n\t"  \
        "popl %es\n\t" \
        "addl $4,%esp\n\t" \
        "iret\n\t"

/*
#define BUILD_USER_IRQ \
        "call " SYMBOL_NAME_STR(build_user_irq) "\n\t"
*/

#define IRQ_ADDR_TABLE_START() \
  __asm__ (".section irq_handlers_addr,\"a\"\n\t" \
           "__start_irq_handlers_addr:\n\t" \
           ".previous\n\t");

#define IRQ_ADDR_TABLE_END() \
  __asm__ (".section irq_handlers_addr,\"a\"\n\t" \
           "__end_irq_handlers_addr:\n\t" \
           ".long -1\n\t" \
           ".previous\n\t");

extern void *__start_irq_handlers_addr;
extern void *__end_irq_handlers_addr;

#define IRQ_NAME(irq) irq_handler_##irq(void)

#define BUILD_IRQ(irq) \
asmlinkage void IRQ_NAME(irq); \
  __asm__ (".section irq_handlers_addr,\"a\"\n\t" \
           ".align 4\n\t" \
           ".long "SYMBOL_NAME_STR(irq_handler_) #irq "\n\t" \
           ".text\n\t" \
           "\n"__ALIGN_STR"\n" \
           SYMBOL_NAME_STR(irq_handler_) #irq ":\n\t" \
           "pushl $"#irq"-256\n\t" \
           "jmp " SYMBOL_NAME_STR(common_irq_body) "\n\t")

#define BUILD_COMMON_IRQ_BODY() \
  __asm__ (".text\n\t" \
           "\n" __ALIGN_STR"\n" \
           SYMBOL_NAME_STR(common_irq_body) ":\n\t" \
           "cld\n\t" \
           HW_SAVE_ALL \
           "call " SYMBOL_NAME_STR(irq_handler) "\n\t" \
           "testl %eax,%eax\n\t" \
           "jnz  1f\n\t" \
           HW_RESTORE_ALL \
           "1: cld\n\t" \
           "jmp *(" SYMBOL_NAME_STR(XM_root_func) " + 20)\n")

#define BUILD_LOCAL_APIC_IRQ(irq) \
 __asm__ (".section irq_handlers_addr,\"a\"\n\t" \
           ".align 4\n\t" \
           ".long "SYMBOL_NAME_STR(irq_handler_) #irq "\n\t" \
           ".text\n\t" \
           "\n"__ALIGN_STR"\n" \
           SYMBOL_NAME_STR(irq_handler_) #irq ":\n\t" \
           "pushl $"#irq"-256\n\t" \
           HW_SAVE_ALL \
           "cld\n\t" \
           "movl %esp, %eax\n\t" \
           "pushl %eax\n\t" \
           "call local_apic_irq\n\t" \
           "addl $4,%esp\n\t" \
           HW_RESTORE_ALL);

#define TRAP_ADDR_TABLE_START() \
  __asm__ (".section trap_handlers_addr,\"a\"\n\t" \
           "__start_trap_handlers_addr:\n\t" \
           ".previous\n\t");

#define TRAP_ADDR_TABLE_END() \
  __asm__ (".section trap_handlers_addr,\"a\"\n\t" \
           "__end_trap_handlers_addr:\n\t" \
           ".long -1\n\t" \
           ".previous\n\t");

extern void *__start_trap_handlers_addr;
extern void *__end_trap_handlers_addr;

#define BUILD_TRAP_NAME(trapnr)  trap_##trapnr(void)

#define BUILD_TRAP_ERRCODE(trapnr) \
asmlinkage void BUILD_TRAP_NAME(trapnr); \
__asm__ (".section trap_handlers_addr,\"a\"\n\t" \
	 ".align 4\n\t" \
         ".long "SYMBOL_NAME_STR(trap_) #trapnr "\n\t" \
         ".text\n\t" \
         "\n" __ALIGN_STR"\n\t" \
         SYMBOL_NAME_STR(trap_) #trapnr ":\n\t" \
         "cld\n\t" \
         HW_SAVE_ALL \
	 "pushl $"#trapnr"\n\t" \
	 "call " SYMBOL_NAME_STR(trap_handler) "\n\t" \
	 "addl $4,%esp\n\t" /* popl trapnr */ \
	 "testl %eax,%eax\n\t" \
         "popl %ebx\n\t" \
         "popl %ecx\n\t" \
         "popl %edx\n\t" \
         "popl %esi\n\t" \
         "popl %edi\n\t" \
         "popl %ebp\n\t" \
         "jnz 1f\n\t" \
         "popl %eax\n\t" \
         "popl %ds\n\t"  \
         "popl %es\n\t" \
         "addl $4,%esp\n\t" /* popl error code */ \
	 "iret\n" \
	 "1:\n\t" \
         "movl ("SYMBOL_NAME_STR(root_idt_table + 4 * trapnr)"),%eax\n\t" \
         "mov 8(%esp),%es\n\t" \
         "movl %eax,8(%esp)\n\t" \
         "popl %eax\n\t" \
         "popl %ds\n\t" \
         "ret\n\t")

#define BUILD_TRAP_NOERRCODE(trapnr) \
asmlinkage void BUILD_TRAP_NAME(trapnr); \
__asm__ (".section trap_handlers_addr,\"a\"\n\t" \
	 ".align 4\n\t" \
         ".long "SYMBOL_NAME_STR(trap_) #trapnr "\n\t" \
         ".text\n\t" \
         "\n" __ALIGN_STR"\n\t" \
         SYMBOL_NAME_STR(trap_) #trapnr ":\n\t" \
         "cld\n\t" \
         "pushl $0\n\t" /* error code = 0 */ \
         HW_SAVE_ALL \
	 "pushl $"#trapnr"\n\t" \
	 "call " SYMBOL_NAME_STR(trap_handler) "\n\t" \
	 "addl $4,%esp\n\t" /* popl trapnr */ \
         "testl %eax,%eax\n\t" \
         "popl %ebx\n\t" \
         "popl %ecx\n\t" \
         "popl %edx\n\t" \
         "popl %esi\n\t" \
         "popl %edi\n\t" \
         "popl %ebp\n\t" \
         "jnz 1f\n\t" \
         "popl %eax\n\t" \
         "popl %ds\n\t"  \
         "popl %es\n\t" \
         "addl $4,%esp\n\t" /* popl error code */ \
	 "iret\n" \
	 "1:\n\t" \
         "movl ("SYMBOL_NAME_STR(root_idt_table + 4 * trapnr)"),%eax\n\t" \
         "movl %eax,12(%esp)\n\t" \
         "popl %eax\n\t" \
         "popl %ds\n\t" \
         "popl %es\n\t" \
         "ret\n\t")

#define SYSTEM_CALL_HANDLER_ASM(syscall) \
  system_call_handler_asm_##syscall

#define BUILD_SYSTEM_CALL_HANDLER_ASM(syscall, syscall_entries) \
asmlinkage void system_call_handler_asm_##syscall(void); \
__asm__ (".text\n\t" \
         "\n" __ALIGN_STR"\n\t" \
	 SYMBOL_NAME_STR(system_call_handler_asm_) #syscall ":\n\t" \
         "pushl %eax\n\t" \
         "cld\n\t" \
	 HW_SAVE_ALL \
         "cmpl $("SYMBOL_NAME_STR(syscall_entries)"),%eax\n\t" \
	 "jae 1f\n\t" \
         "movl "SYMBOL_NAME_STR(syscall_table)" (,%eax,4), %eax\n\t" \
         "testl %eax, %eax\n\t" \
         "jz 1f\n\t" \
         "call *%eax\n\t" \
         "movl %eax, 0x18(%esp)\n\t" \
	 HW_RESTORE_ALL \
         "1:\n\t" \
         "pushl 0x18(%esp)\n\t" \
	 "pushl $unknown_syscall\n\t" \
         "call printk\n\t" \
         "addl $0x8, %esp\n\t" \
         "movl $-1, 0x18(%esp)\n\t" \
	 HW_RESTORE_ALL \
         "unknown_syscall:\n\t" \
         ".asciz \"System call (\%d) unknown\\n\"\n\t");

#define INTERCEPT_SYSTEM_CALL_HANDLER_ASM(syscall) \
  intercept_system_call_handler_asm_ ##syscall

#define BUILD_INTERCEPT_SYSTEM_CALL_HANDLER_ASM(syscall) \
asmlinkage void intercept_system_call_handler_asm_ ##syscall(void); \
__asm__ (".text\n\t" \
         "\n" __ALIGN_STR"\n\t" \
	 SYMBOL_NAME_STR(intercept_system_call_handler_asm_) #syscall  ":\n\t" \
         "pushl %eax\n\t" \
         "cld\n\t" \
	 HW_SAVE_ALL \
	 "call " SYMBOL_NAME_STR(root_system_call)"\n\t" \
         "testl %eax,%eax\n\t" \
         "popl %ebx\n\t" \
         "popl %ecx\n\t" \
         "popl %edx\n\t" \
         "popl %esi\n\t" \
         "popl %edi\n\t" \
         "popl %ebp\n\t" \
         "jnz 1f\n\t" \
         "popl %eax\n\t" \
         "popl %ds\n\t"  \
         "popl %es\n\t" \
         "addl $4,%esp\n\t" /* popl %eax */ \
	 "iret\n" \
	 "1:\n\t" \
         "movl ("SYMBOL_NAME_STR(root_idt_table + 4 * syscall)"),%eax\n\t" \
         "movl %eax,12(%esp)\n\t" \
         "popl %eax\n\t" \
         "popl %ds\n\t" \
         "popl %es\n\t" \
         "ret\n\t")

#define FAKE_REGS { 0, 0, 0, 0, 0, 0, 0, __KERNEL_DS, 0, 0, \
  (long)dummy_f, __KERNEL_CS, X86_EFLAGS_IF, 0, 0 }

#define SET_PT_REG_IRQ(pt_regs, irq) pt_regs.orig_eax = irq

#include <domain.h>

/*
extern unsigned long _restore_event_stack_code;

// setup_irq manipulates the user and supervisor stack to execute
// an interrupt the next time the user code is executed
static inline void setup_uirq (struct pt_regs *regs, int event, 
			       void (*handler) (int, struct pt_regs *)) {
  unsigned long esp3;

  esp3 = regs -> esp;
  
  // Here the user stack is manipulated to call
  // the user's handlers
  // firstly we insert the original return point
  // it is necessary to return to the same point where
  // the interrupt were
  
  esp3 -= 4;
  *((unsigned long *) esp3) = regs -> eip;
  // Next code will pop the interrupt handler's parameters
  // from the stack when the interrupt handler returns, it is
  // embedded in the stack.
  // the exact code used is:
  // 0xc30cc483, whose translation in assembly is:
  // ----------------
  // add $0xc, %esp ; 0x83c40c --> where $c is coded by the last c
  // ret            ; 0xc3
  // -----------------
  // recall that intel architecture uses the little endian
  // method to store information
  //esp3 -= 4;
  // *((unsigned long *) esp3) = 0xc30cc483;
  //ret_stack_addr = esp3;
  // now it is the turn for the interrupt handler's parameter
  esp3 -= 4;
  *((unsigned long *) esp3) = 0;
  esp3 -= 4;
  *((unsigned long *) esp3) = event;
  
  // when the interrupt handler returns, its parameters have to
  // be popped from the stack
  esp3 -= 4;
  *((unsigned long *) esp3) = _restore_event_stack_code;//ret_stack_addr;
  
  // the address of the interrupt handler
  
  regs -> eip = (unsigned long) handler;
  regs -> esp = esp3;
}

// Next code will pop the interrupt handler's parameters
// from the stack when the interrupt handler returns, it is
// embedded in the stack.
// the exact code used is:
// 0xc30cc483, whose translation in assembly is:
// ----------------
// add $0xc, %esp ; 0x83c40c --> where $c is coded by the last c
// ret            ; 0xc3
// -----------------
// recall that intel architecture uses the little endian
// method to store information
#define RESTORE_EVENT_STACK() 0xc308c483
*/

#endif
#endif
