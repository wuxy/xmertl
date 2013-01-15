/*
 * $FILE: irqs.c
 *
 * IRQs related functions
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

#include <linux_inc.h>
#include <irqs.h>
#include <domain.h>
#include <sched.h>
#include <timer.h>
#include <arch/cpu.h>
#include <xmtrace.h>

__inline__ void __hw_sti (void) {
  __asm__ __volatile__("sti": : :"memory");
}

__inline__ void __hw_cli (void) {
  __asm__ __volatile__("cli": : :"memory");
}

__inline__ void __hw_save_flags (unsigned long *flags) {
  __asm__ __volatile__ ("pushfl ; popl %0":"=g" (*flags): /* no input */);  
}

__inline__ void __hw_restore_flags (unsigned long flags) {
  __asm__ __volatile__("pushl %0 ; popfl": /* no output */ 
		       :"g" (flags):"memory", "cc");
}

__inline__ void __hw_save_flags_and_cli (unsigned long *flags) {
  __hw_save_flags(flags);
  __hw_cli ();
}
__inline__ int __hw_is_cli (void) {
  unsigned long flags;
  __hw_save_flags (&flags);
  return !(flags & X86_EFLAGS_IF);
}

void hw_sti (void) {
  __hw_sti ();
  rtl_trace2(RTL_TRACE_HARD_STI, 0);
}

void hw_cli (void) {
  __hw_cli ();
  rtl_trace2 (RTL_TRACE_HARD_CLI, 0);
}

void hw_save_flags (unsigned long *flags) {
  __hw_save_flags (flags);	
  rtl_trace2 (RTL_TRACE_HARD_SAVE_FLAGS, *flags);
}

void hw_restore_flags (unsigned long flags) {
  __hw_restore_flags (flags);	
  rtl_trace2 (RTL_TRACE_HARD_RESTORE_FLAGS, flags);
}

void hw_save_flags_and_cli (unsigned long *flags) {
  hw_save_flags (flags);
  hw_cli ();
}

int hw_is_cli (void) {
  unsigned long flags;
  __hw_save_flags (&flags);
  return !(flags & X86_EFLAGS_IF);
}


struct desc_struct *real_idt_table;
static struct desc_struct *root_idt_table [IDT_ENTRIES];
static void (*__root_cli) (void) = NULL;
static int (*__root_is_cli) (void) = NULL;
static void (*__root_sti) (void) = NULL;
static void (*__root_save_flags) (unsigned long *flags) = NULL;
static void (*__root_restore_flags) (unsigned long flags) = NULL;
static void (**irq_addr)(void);
static void (**trap_addr)(void);
extern asmlinkage void intercept_system_call_handler_asm_0x80(void);
extern asmlinkage void system_call_handler_asm_0x82 (void);

static void vcli (void) {
  disable_events_flag (xm_root_domain);
  //  hw_cli ();
}

extern volatile int xmprint;
extern void write_printkbuf (void);
static void vsti (void) {
  enable_events_flag (xm_root_domain);
  sync_events ();

  if (xmprint)
    write_printkbuf();
    
  //hw_sti ();
}

static void vsave_flags (unsigned long *flags) {
  hw_save_flags (flags);
  
  *flags = (is_events_flag_enabled (xm_root_domain))? 
    *flags | X86_EFLAGS_IF : *flags & ~(X86_EFLAGS_IF);
}

static void vrestore_flags (unsigned long flags) {
  unsigned long i_enabled = flags & X86_EFLAGS_IF;
  
//  hw_restore_flags ((flags | X86_EFLAGS_IF));
  
  if (i_enabled)
    vsti ();
  else
    vcli ();
}

static int vis_cli (void) {
  return !is_events_flag_enabled (xm_root_domain);
}

/* This function is always executed with interrupts disabled, isn't it? */
int irq_handler (struct pt_regs regs) {
  int irq = regs.orig_eax & 0xff;
  int execute_root_ret_from_intr;
  
  hw_xpic[irq]->ack(irq);
  if (irq != hwtimer.timer_event) {
    set_bit (xm_domain_list -> events -> pending_events, irq);
  } else {
    timer_handler();
    hw_xpic[irq]->end(irq);
  }
  // xm_sched is called to execute the suitable handlers
  // it returns "1" if the root irq handler has been executed
  execute_root_ret_from_intr = 
    (xm_sched () && (xm_current_domain == xm_root_domain));

  //hw_xpic[irq] -> end (irq);
  return execute_root_ret_from_intr;
}

int trap_handler (int trap, struct pt_regs regs) {
  if (xm_current_domain -> events -> trap_handler [trap])
    (*xm_current_domain -> events -> trap_handler [trap]) (trap, &regs);
  
  // return 1 if the root trap handler must to be executed
  return (xm_current_domain == xm_root_domain);
}

int root_system_call (struct pt_regs regs) {
  int syscall = regs.orig_eax;

  if (xm_current_domain != xm_root_domain)
    printk ("<Root syscall %d>: " \
            "Only the root domain can call root domain syscalls\n",\
            syscall);
  
  return (xm_current_domain == xm_root_domain);
}

static void emulate_iret (void) {
  enable_events_flag (xm_root_domain);
}

void show_trap_info (int trap, struct pt_regs *regs) {
  int cr2;
  printk ("<Domain \"%s\" killed> trap %d errorcode %d not handled\n",
          xm_current_domain -> name, trap, (int)regs -> orig_eax);
  printk ("\nCurrent CPU state:\n");
  printk ("EIP: 0x%x:[<0x%x>]", (unsigned int) 0xffff & regs->xcs,
	  (unsigned int) regs->eip);
  
  /* if (regs->xcs & 3)
     printk (" ESP: 0x%x:0x%x", (unsigned int) 0xffff & regs->xss,
     (unsigned int) regs->esp);
  */
  printk (" EFLAGS: 0x%x  \n", (unsigned int) regs->eflags);
  printk ("EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x\n",
	  (unsigned int) regs->eax, (unsigned int) regs->ebx,
	  (unsigned int) regs->ecx, (unsigned int) regs->edx);
  printk ("ESI: 0x%x EDI: 0x%x EBP: 0x%x",
	  (unsigned int) regs->esi, (unsigned int) regs->edi,
	  (unsigned int) regs->ebp);
  printk (" DS: 0x%x ES: 0x%x\n",
	  (unsigned int) 0xffff & regs->xds,
	  (unsigned int) 0xffff & regs->xes);
  
  switch (trap) {
  case 14:
    __asm__ __volatile__ ("mov %%cr2, %%eax\n\t" : "=a" (cr2));
    printk ("Fault address: 0x%x\n", cr2);
    break;
  default:
    break;
  }
  
}

int hw_irq_takeover (void) {
  unsigned long hw_flags, vector, irq;

   // Our irq and trap tables which will replace actual IDT table
  
  irq_addr = (void (**) (void))
    &__start_irq_handlers_addr;
  trap_addr = (void (**) (void))
    &__start_trap_handlers_addr;
  
  real_idt_table = hw_get_idt_table_addr ();
  
  hw_save_flags_and_cli(&hw_flags);
      
  __root_sti = XM_root_func.__sti;
  __root_cli = XM_root_func.__cli;
  __root_save_flags = XM_root_func.__save_flags;
  __root_restore_flags = XM_root_func.__restore_flags;
  __root_is_cli = XM_root_func.__is_cli;

  XM_root_func.__sti = vsti;
  XM_root_func.__cli = vcli;
  XM_root_func.__save_flags = vsave_flags;
  XM_root_func.__restore_flags = vrestore_flags;
  XM_root_func.__is_cli = vis_cli;
  XM_root_func.__emulate_iret = emulate_iret;

  for (vector = 0; vector < IDT_ENTRIES; vector ++)
    root_idt_table [vector] = hw_get_gate_addr (vector);
  
  
  for (irq = 0; irq < NR_IRQS; irq ++) {
    hw_xpic [irq] = ((irq_desc_t *)XM_root_func.__irq_desc)[irq].handler;
    ((irq_desc_t *)XM_root_func.__irq_desc)[irq].handler = &vpic;
  }
  
  // In an i386 there are 16 irqs, 0..15 (besides of the apic
  // interrupt)
   
  for (irq = 0; irq < NR_IRQS; irq++) {
    vector = irq + FIRST_EXTERNAL_VECTOR;
    
    // Replacing all hw irq gates for XtratuM routines
    hw_set_irq_gate(vector, irq_addr [irq]);
  }

  /*
#ifdef CONFIG_X86_LOCAL_APIC
  // setting up the apic interrupt
  hw_set_irq_gate(xm_LOCAL_TIMER_VECTOR, irq_addr [16]);
#endif
  */
  // Replacing all traps and exceptions with the XtratuM traps and exceptions
  // but NMI which will be only dealt by the ROOT domain

  hw_set_trap_gate(0, trap_addr[0]);
  hw_set_trap_gate(1, trap_addr[1]);
  hw_set_sys_gate(3, trap_addr[3]);
  hw_set_sys_gate(4, trap_addr[4]);
  hw_set_sys_gate(5, trap_addr[5]);
  hw_set_trap_gate(6, trap_addr[6]);
  hw_set_trap_gate(7, trap_addr[7]);
  hw_set_trap_gate(8, trap_addr[8]);
  hw_set_trap_gate(9, trap_addr[9]);
  hw_set_trap_gate(10, trap_addr[10]);
  hw_set_trap_gate(11, trap_addr[11]);
  hw_set_trap_gate(12, trap_addr[12]);
  hw_set_trap_gate(13, trap_addr[13]);
  hw_set_irq_gate(14, trap_addr[14]);
  hw_set_trap_gate(15, trap_addr[15]);
  hw_set_trap_gate(16, trap_addr[16]);
  hw_set_trap_gate(17, trap_addr[17]);
  hw_set_trap_gate(18, trap_addr[18]);
  hw_set_trap_gate(19, trap_addr[19]);
    
  // The XM's syscall interrupt
  hw_set_sys_gate (0x82,
  	   SYSTEM_CALL_HANDLER_ASM(0x82));

  // The Root OS sycall, it can not be called when it not in execution
  hw_set_sys_gate (0x80, INTERCEPT_SYSTEM_CALL_HANDLER_ASM(0x80));
  
  hw_restore_flags(hw_flags);

  return 0;
}

int hw_irq_giveup (void) {
  unsigned long hw_flags, vector, irq;

  hw_save_flags_and_cli(&hw_flags);
  
  for (irq = 0; irq < NR_IRQS; irq++) {

    vector = irq + FIRST_EXTERNAL_VECTOR;

    // Restoring all hw irq vectors
    hw_set_irq_gate(vector, root_idt_table [vector]);
  }

  
  for (irq = 0; irq < NR_IRQS; irq ++)
    ((irq_desc_t *)XM_root_func.__irq_desc)[irq].handler = hw_xpic [irq];
  
  /*
#ifdef CONFIG_X86_LOCAL_APIC
  // setting up the apic interrupt
  hw_set_irq_gate(LOCAL_TIMER_VECTOR,
                  root_idt_table [LOCAL_TIMER_VECTOR]);
#endif
  */
  // Restoring all originals traps and exceptions
  
  hw_set_trap_gate(0,root_idt_table[0]);
  hw_set_trap_gate(1,root_idt_table[1]);
  hw_set_sys_gate(3,root_idt_table[3]);
  hw_set_sys_gate(4,root_idt_table[4]);
  hw_set_sys_gate(5,root_idt_table[5]);
  hw_set_trap_gate(6,root_idt_table[6]);
  hw_set_trap_gate(7,root_idt_table[7]);
  hw_set_trap_gate(8,root_idt_table[8]);
  hw_set_trap_gate(9,root_idt_table[9]);
  hw_set_trap_gate(10,root_idt_table[10]);
  hw_set_trap_gate(11,root_idt_table[11]);
  hw_set_trap_gate(12,root_idt_table[12]);
  hw_set_trap_gate(13,root_idt_table[13]);
  hw_set_irq_gate(14,root_idt_table[14]);
  hw_set_trap_gate(15,root_idt_table[15]);
  hw_set_trap_gate(16,root_idt_table[16]);
  hw_set_trap_gate(17,root_idt_table[17]);
  hw_set_trap_gate(18,root_idt_table[18]);
  hw_set_trap_gate(19,root_idt_table[19]);  
  hw_set_sys_gate(0x82,
		  root_idt_table[0x82]);
  hw_set_sys_gate(0x80,root_idt_table[0x80]);

  XM_root_func.__sti = __root_sti;
  XM_root_func.__cli = __root_cli;
  XM_root_func.__is_cli = __root_is_cli;
  XM_root_func.__save_flags = __root_save_flags;
  XM_root_func.__restore_flags =__root_restore_flags;
  XM_root_func.__emulate_iret = 0;
  
  hw_restore_flags(hw_flags);

  return 0;
}


// IRQ table

IRQ_ADDR_TABLE_START();
BUILD_IRQ(0x00); BUILD_IRQ(0x01); BUILD_IRQ(0x02);
BUILD_IRQ(0x03); BUILD_IRQ(0x04); BUILD_IRQ(0x05);
BUILD_IRQ(0x06); BUILD_IRQ(0x07); BUILD_IRQ(0x08);
BUILD_IRQ(0x09); BUILD_IRQ(0x0a); BUILD_IRQ(0x0b);
BUILD_IRQ(0x0c); BUILD_IRQ(0x0d); BUILD_IRQ(0x0e);
BUILD_IRQ(0x0f);

#ifdef CONFIG_X86_LOCAL_APIC
#define LOCAL_TIMER_VECTOR  0x30
BUILD_LOCAL_APIC_IRQ(0x30);
#endif
IRQ_ADDR_TABLE_END();

BUILD_COMMON_IRQ_BODY();

// Trap table
TRAP_ADDR_TABLE_START();
BUILD_TRAP_NOERRCODE(0x0); BUILD_TRAP_NOERRCODE(0x1);
BUILD_TRAP_NOERRCODE(0x2); BUILD_TRAP_NOERRCODE(0x3);
BUILD_TRAP_NOERRCODE(0x4); BUILD_TRAP_NOERRCODE(0x5);
BUILD_TRAP_NOERRCODE(0x6); BUILD_TRAP_NOERRCODE(0x7);
BUILD_TRAP_ERRCODE(0x8); BUILD_TRAP_NOERRCODE(0x9);
BUILD_TRAP_ERRCODE(0xa); BUILD_TRAP_ERRCODE(0xb);
BUILD_TRAP_ERRCODE(0xc); BUILD_TRAP_ERRCODE(0xd);
BUILD_TRAP_ERRCODE(0xe); BUILD_TRAP_NOERRCODE(0xf);
BUILD_TRAP_NOERRCODE(0x10); BUILD_TRAP_ERRCODE(0x11);
BUILD_TRAP_NOERRCODE(0x12); BUILD_TRAP_NOERRCODE(0x13);
BUILD_TRAP_ERRCODE(0x14); BUILD_TRAP_ERRCODE(0x15);
BUILD_TRAP_ERRCODE(0x16); BUILD_TRAP_ERRCODE(0x17);
BUILD_TRAP_ERRCODE(0x18); BUILD_TRAP_ERRCODE(0x19); 
BUILD_TRAP_ERRCODE(0x1a); BUILD_TRAP_ERRCODE(0x1b);
BUILD_TRAP_ERRCODE(0x1c); BUILD_TRAP_ERRCODE(0x1d);
BUILD_TRAP_ERRCODE(0x1e); BUILD_TRAP_ERRCODE(0x1f);
TRAP_ADDR_TABLE_END();

BUILD_SYSTEM_CALL_HANDLER_ASM(0x82, SYSCALL_ENTRIES);

BUILD_INTERCEPT_SYSTEM_CALL_HANDLER_ASM(0x80);
