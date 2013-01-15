/*
 * $FILE: cpu.h
 *
 * CPU related functions
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

#ifndef _XM_ARCH_CPU_H_
#define _XM_ARCH_CPU_H_

#include <linux_inc.h>

#define load_pd(pd) {\
  __asm__ __volatile__ ("movl %0,%%cr3": :"r" (__pa(pd))); \
}

#define save_pd(pd) {\
  __asm__ __volatile__ ("movl %%cr3, %0\n\t": "=r" (pd) :); \
  pd = (unsigned long) __va (pd); \
}

#ifdef CONFIG_FP_SUPPORT
#define init_fp() __asm__ __volatile__ ("clts\n\t" \
	                                "fninit\n\t" ::)
#else
#define init_fp()
#endif

#define init_sstack(stack, entry_point, wrapper) {\
  *--(stack) = (unsigned long) entry_point; \
  *--(stack) = (unsigned long) 0; \
  *--(stack) = (unsigned long) wrapper; \
}

/*
#define init_ustack(stack, events_user_addr) {\
  *--(stack) = (unsigned long) xm_current_domain -> heap; \
  *--(stack) = (unsigned long) DEFAULT_HEAP_SIZE; \
  *--(stack) = (unsigned long) events_user_addr; \
}
*/

#define hw_save_stack(stack) \
  __asm__ __volatile__ ("movl %%esp, %0\n\t" : "=r" (stack));

#define PUSH_REGISTERS() \
  __asm__ __volatile__("pushl %%eax\n\t" \
                       "pushl %%ebp\n\t" \
                       "pushl %%edi\n\t" \
                       "pushl %%esi\n\t" \
                       "pushl %%edx\n\t" \
                       "pushl %%ecx\n\t" \
                       "pushl %%ebx\n\t"::)

#ifdef CONFIG_FP_SUPPORT
#define PUSH_FP() \
  __asm__ __volatile__("movl %%cr0, %%eax\n\t" \
                       "andl $8, %%eax\n\t" \
                       "pushl %%eax\n\t" \
                       "clts\n\t" \
                       "sub $108,%%esp\n\t" \
                       "fnsave (%%esp)\n\t" \
                       "fwait\n\t"::)

#define POP_FP() \
  __asm__ __volatile__("clts\n\t" \
                       "frstor (%%esp)\n\t" \
                       "add $108, %%esp\n\t" \
                       "popl %%eax\n\t" \
                       "testl %%eax, %%eax\n\t" \
                       "jz 11f\n\t" \
                       "movl %%cr0, %%edx\n\t" \
                       "orl %%edx, %%eax\n\t" \
		       "movl %%eax, %%cr0\n\t" \
                       "11:\n\t"::)
#else

#define PUSH_FP()
#define POP_FP()

#endif

#define POP_REGISTERS() \
  __asm__ __volatile__("popl %%ebx\n\t" \
                       "popl %%ecx\n\t" \
                       "popl %%edx\n\t" \
                       "popl %%esi\n\t" \
                       "popl %%edi\n\t" \
                       "popl %%ebp\n\t" \
                       "popl %%eax\n\t"::)

#define PAGE_OFFSET_STR "0xC0000000"

#define CHANGE_DOMAIN(new_domain, current_domain) \
  __asm__ __volatile__("movl (%%ebx), %%edx\n\t" \
                       "pushl $1f\n\t" \
                       "movl %%esp, (%%edx)\n\t" \
                       "movl (%%ecx), %%esp\n\t" \
                       "movl 4(%%ecx), %%eax\n\t" \
                       "subl $"PAGE_OFFSET_STR", %%eax\n\t" \
                       "movl %%eax, %%cr3\n\t" \
                       "movl %%ecx, (%%ebx)\n\t" \
                       "ret\n\t" \
                       "1:\n\t" : :"c" (new_domain), \
                       "b" (current_domain))

#define domain_context_switch(new_domain, current_domain) \
  PUSH_REGISTERS(); \
  PUSH_FP(); \
  CHANGE_DOMAIN(new_domain, current_domain); \
  POP_FP(); \
  POP_REGISTERS();

#define JUMP_XM_DOM(entry_point, domain) \
  __asm__ __volatile__ ("pushl %1\n\t" \
                        "call *%0\n\t" \
			"addl $0x4, %%esp\n\t":: "g" (entry_point), \
			"g" (domain -> events_user_addr))

/*
#define JUMP_USER_LEVEL(entry_point, user_stack) \
  __asm__ __volatile__ ("sti\n\t" \
                        "pushl %1\n\t" \
                        "pushl %2\n\t" \
                        "pushl %0\n\t" \
                        "pushl %3\n\t" \
                        "movl %1,%%eax\n\t" \
                        "movl %%eax,%%ds\n\t" \
                        "movl %%eax,%%es\n\t" \
                        "movl $0,%%ebp\n\t" \
                        "lret\n\t" \
                        :: "g" (__USER_CS), "g" (__USER_DS), \
                        "g" (user_stack), "g" (entry_point))

extern struct tss_struct *tss;

#define save_tss_desc(tss) __asm__ __volatile__ ("str %0\n\t" : "=r" (tss))
#define save_tss_gdt_desc_index(tss) {\
  __asm__ __volatile__ ("str %0\n\t" : "=r" (tss)); \
  tss = (tss >> 3); \
}
*/

#define save_gdt_base(gdt_base) { \
  unsigned long gdt [2]; \
  __asm__ __volatile__ ("sgdt %0" : "=m" (gdt)); \
  gdt[0] >>= 16; \
  gdt[1] <<= 16; \
  gdt[0] |= gdt[1]; \
  gdt_base = gdt [0]; \
}

/*
static inline void get_tss_struct (struct tss_struct **tss) {
  unsigned short tss_index;
  unsigned long gdt_base;
  struct desc_struct *gdt_table;

  save_gdt_base(gdt_base);
  save_tss_gdt_desc_index (tss_index);
  gdt_table = (struct desc_struct *) gdt_base;

  *tss = (struct tss_struct *) ((gdt_table[tss_index].a >> 16) |
                                ((gdt_table[tss_index].b & 0xFF) << 16) |
                                (gdt_table[tss_index].b & 0xFF000000));
}


#define save_tss_esp0(d) d->irq_sstack = (unsigned long *)tss->esp0
#define load_tss_esp0(d) tss->esp0 = (unsigned long)d->irq_sstack

#define save_sstack(d) save_tss_esp0(d)
#define load_sstack(d) load_tss_esp0(d)
*/

extern void arch_setup (void);

#endif
