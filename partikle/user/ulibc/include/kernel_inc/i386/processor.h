/*
 * $FILE: processor.h
 *
 * Processor related functions: Context switch, Enable/Inable
 * interrupts, etc
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

#ifndef _ARCH_PROCESSOR_H_
#define _ARCH_PROCESSOR_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

// Number of GDT's entries
#define GDT_ENTRIES 12
// PaRTiKle's code segment (Ring 0)
#define KERNEL_CS 0x8
// PaRTiKle's data segment (Ring 0)
#define KERNEL_DS 0x10
// Application's code segment (Ring 0)
#define APP_CS 0x18
// Application's data segment (Ring 0)
#define APP_DS 0x20

// PaRTiKle's TSS segment
#define TSS_SEGMENT 0x28

#define STACK_ADDR 0xBCE00000

#ifndef __ASSEMBLY__

#define IDLE

#define hw_cli() __asm__ __volatile__ ("cli\n\t":::"memory")
#define hw_sti() __asm__ __volatile__ ("sti\n\t":::"memory")

#define hw_restore_flags(flags) \
  __asm__ __volatile__("pushl %0\n\t" \
                       "popfl\n\t": :"g" (flags):"memory")

#define hw_save_flags(flags) \
  __asm__ __volatile__("pushfl\n\t" \
                       "popl %0\n\t" :"=g" (flags): :"memory")

#define hw_save_flags_and_cli(flags) {\
  hw_save_flags(flags); \
  hw_cli(); \
}

static inline int hw_are_interrupts_enabled (void) {
  unsigned int flags;
  hw_save_flags (flags);
  return (flags & 0x200);
}

#define hw_get_stack(stack) __asm__ __volatile__ ("movl %%esp, %0\n\t" : "=m" (stack))
#define hw_set_stack(stack) __asm__ __volatile__ ("movl %0, %%esp\n\t" ::"m" (stack))

#define hw_push_stack(r) __asm__ __volatile__ ("pushl %0\n\t" : : "m" (r))
#define hw_pop_stack(r) __asm__ __volatile__ ("pop %0\n\t" : "=m" (r))

#define CR0_WP 0x10000
#define CR4_PSE 0x0010
#define CR4_PGE 0x0080

#define cpu_has_pse() ({ \
  extern unsigned int cpu_fflags; \
  (cpu_fflags & (1 << 17)); \
})

#define cpu_has_pge() ({ \
  extern unsigned int cpu_fflags; \
  (cpu_fflags & (1 << 13)); \
})

// Definition of a GDT descriptor.

typedef struct gdt_desc_struct {
  unsigned int high, low;
} gdt_desc_t;


// PaRTiKle's GDT table.
extern gdt_desc_t gdt [GDT_ENTRIES];

// According the Intel System programming manual, it is necessary to
// declare and initialise at least one TSS, it stores the different
// stacks for the different protection levels.
typedef struct tss_struct {
  int back_link; /* segment number of previous task,
                         if nested */
  int esp0; /* initial stack pointer ... */
  int ss0; /* and segment for ring 0 */
  int esp1; /* initial stack pointer ... */
  int ss1; /* and segment for ring 1 */
  int esp2; /* initial stack pointer ... */
  int ss2; /* and segment for ring 2 */
  int cr3; /* CR3 - page table directory
              physical address */
  int eip;
  int eflags;
  int eax;
  int ecx;
  int edx;
  int ebx;
  int esp; /* current stack pointer */
  int ebp;
  int esi;
  int edi;
  int es;
  int cs;
  int ss; /* current stack segment */
  int ds;
  int fs;
  int gs;
  int ldt; /* local descriptor table segment */
  unsigned short  trace_trap; /* trap on switch to this task */
  unsigned short  io_bit_map_offset;
  /* offset to start of IO permission
     bit map */
} tss_t;

extern tss_t tss;

#define set_tr(seg) \
  __asm__ __volatile__ ("ltr %0" : : "rm" ((unsigned short)(seg)))

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
  __asm__ __volatile__("sub $108,%%esp\n\t" \
                       "fnsave (%%esp)\n\t" \
                       "fwait\n\t"::)

#define POP_FP() \
  __asm__ __volatile__("frstor (%%esp)\n\t" \
                       "add $108, %%esp\n\t" ::)
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

#define CHANGE_THREAD(new_thread, current_thread) \
  __asm__ __volatile__("movl (%%ebx), %%edx\n\t" \
                       "pushl $1f\n\t" \
                       "movl %%esp, (%%edx)\n\t" \
                       "movl (%%ecx), %%esp\n\t" \
                       "movl %%ecx, (%%ebx)\n\t" \
                       "ret\n\t" \
                       "1:\n\t" : :"c" (new_thread), \
                       "b" (current_thread))

#define context_switch(new_thread, current_thread) \
  PUSH_REGISTERS(); \
  PUSH_FP(); \
  CHANGE_THREAD(new_thread, current_thread); \
  POP_FP(); \
  POP_REGISTERS();

#define rdtsc(low,high) \
  __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

#define JUMP_APP(entry_point) \
  __asm__ __volatile__ ("movl %2, %%eax\n\t" \
                        "pushl %1\n\t" \
                        "pushl $0\n\t" \
                        "pushl %0\n\t" \
                        "pushl %%eax\n\t" \
                        "movl %1, %%eax\n\t" \
                        "movl %%eax, %%ds\n\t" \
                        "movl %%eax, %%es\n\t" \
		  	"movl %%eax, %%ss\n\t" \
                        "movl $0, %%ebp\n\t" \
                        "lret\n\t" \
                        :: "g" (APP_CS), "g" (APP_DS), "g" (entry_point))

/*
 * XXX: routine inserted into the stack
 */

/*
  0:   58                      pop    %eax
  1:   ff d0                   call   *%eax
  3:   83 c4 0c                add    $0xc,%esp
  6:   cb                      lret
*/

#define CALL_SIGNAL_HANDLER(hdl, signal, sinfo, void_p) { \
  __asm__ __volatile__ ("pushl $0xcb0cc4\n\t" \
                        "pushl $0x83d0ff58\n\t" \
                        "movl %%esp, %%eax\n\t" \
                        "pushl $0x8\n\t" \
                        "pushl $1f\n\t" \
                        "pushl %%edx\n\t" \
                        "pushl %%ecx\n\t" \
                        "pushl %%ebx\n\t" \
                        "pushl %0\n\t" \
                        "pushl $0x18\n\t" \
                        "pushl %%eax\n\t" \
                        "lret\n\t" \
                        "1:\n\t" \
		        "addl $0x8, %%esp\n\t": \
			                      : "m" (hdl), "b" (signal), \
                                                  "c" (sinfo), "d" (void_p) \
                                              : "eax"); \
}

/*
 * XXX: routine inserted into the stack
 */

/*
  0:   58                      pop    %eax
  1:   ff d0                   call   *%eax
  3:   83 c4 04                add    $0x4,%esp
  6:   cb                      lret
*/

#define CALL_CANCEL_HANDLER(hdl, arg) { \
  __asm__ __volatile__ ("pushl $0xcb04c4\n\t" \
                        "pushl $0x83d0ff58\n\t" \
                        "movl %%esp, %%eax\n\t" \
                        "pushl $0x8\n\t" \
                        "pushl $1f\n\t" \
                        "pushl %%ecx\n\t" \
                        "pushl %%ebx\n\t" \
                        "pushl $0x18\n\t" \
                        "pushl %%eax\n\t" \
                        "lret\n\t" \
                        "1:\n\t" \
		        "addl $0x8, %%esp\n\t":  \
			                      : "b" (hdl), "c" (arg) \
                                              : "eax"); \
}

#else

#define PUSH_REGISTERS() \
  pushl %eax; \
  pushl %ebp; \
  pushl %edi; \
  pushl %esi; \
  pushl %edx; \
  pushl %ecx; \
  pushl %ebx;

#define POP_REGISTERS() \
  popl %ebx; \
  popl %ecx; \
  popl %edx; \
  popl %esi; \
  popl %edi; \
  popl %ebp; \
  popl %eax;

#endif

#endif
