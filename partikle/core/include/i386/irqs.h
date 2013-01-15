/*
 * $FILE: irqs.h
 *
 * Arch-dependent part of the interruptions and traps
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

#ifndef _ARCH_IRQS_H_
#define _ARCH_IRQS_H_
#ifndef __ASSEMBLY__
// registers after an interrupt/trap
/*
 * esp and ss are not stored automatically by the processor
 */
typedef struct context_struct {
  /* saved registers */
  long ebx;
  long ecx;
  long edx;
  long esi;
  long edi;
  long ebp;
  long eax;
  int xds;
  int xes;
  /* irq or trap raised */
  long irqnr;
  /* error code pushed by the processor, 0 if none */
  long error_code;
  /* processor state frame */
  long eip;
  int  xcs;
  long eflags;
  long esp;
  long xss;
} context_t;
#endif

#ifdef _KERNEL_
// existing entries in the idt_table
#define IDT_ENTRIES 256

/* existing traps (just 32 in the ia32 arch) */
#define TRAP_NR 32

/* existing hw interrupts (dealt by the PIC and APIC) */
#define FIRST_EXTERNAL_VECTOR 0x20
#ifndef LOCAL_APIC
#define FIRST_USER_IRQ_VECTOR 0x30
#define IRQ_NR 16
#else
#define FIRST_USER_IRQ_VECTOR 0x31
#define IRQ_NR 17
#endif // !LOCAL_APIC
#define NR_USER_IRQS (IDT_ENTRIES - TRAP_NR - IRQ_NR)

#ifndef __ASSEMBLY__
// an entry in the IDT table
typedef struct idt_desc_struct {
  unsigned int high, low;
} idt_desc_t;

// kernel idt
extern idt_desc_t idt [IDT_ENTRIES];

/* addresses of the .irq_handlers_table and  .trap_handlers_table */
extern void *_sirq_handlers;
extern void *_eirq_handlers;

extern void *_strap_handlers;
extern void *_etrap_handlers;

#define hw_set_gate(gate_addr,type,dpl,addr) ({ \
  int __d0, __d1; \
  __asm__ __volatile__ ("movw %%dx,%%ax\n\t" \
       "movw %4,%%dx\n\t" \
       "movl %%eax,%0\n\t" \
       "movl %%edx,%1" \
       :"=m" (*((long *) (gate_addr))), \
        "=m" (*(1+(long *) (gate_addr))), "=&a" (__d0), "=&d" (__d1) \
       :"i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
        "3" ((char *) (addr)),"2" (KERNEL_CS << 16)); \
})

#define hw_set_irq_gate(vector,addr) \
  hw_set_gate ((unsigned long)idt + vector * sizeof (idt_desc_t), 14, 0, addr)

#define hw_set_trap_gate(vector,addr) \
  hw_set_gate ((unsigned long)idt + vector * sizeof (idt_desc_t), 15, 0, addr)

#define hw_set_sys_gate(vector,addr) \
  hw_set_gate ((unsigned long)idt + vector * sizeof (idt_desc_t), 15, 3, addr)

#endif // !__ASSEMBLY__

#endif
#endif
