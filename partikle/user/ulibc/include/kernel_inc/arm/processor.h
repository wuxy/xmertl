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

/* PSR bits. CPSR and SPSR */
#define PSR_I           0x80            /* Interrupt enable/disable */
#define PSR_F           0x40            /* FIQ Interrupt enable/disable */
#define PSR_T           0x20            /* Architecture Version */
#define PSR_CCMASK      0xf0000000      /* Condition code mask */

#define PSR_MODE 0x1f            /* mode mask */
#define PSR_USR32_MODE 0x10
#define PSR_FIQ32_MODE 0x11
#define PSR_IRQ32_MODE 0x12
#define PSR_SVC32_MODE 0x13
#define PSR_ABT32_MODE 0x17
#define PSR_UND32_MODE 0x1b
#define PSR_32_MODE 0x10

#ifndef __ASSEMBLY__

#define IDLE

static inline unsigned long get_cpsr(void) {
  unsigned long _cpsr;
  __asm__ __volatile__ ("mrs %0, cpsr_all\n\t" : "=r" (_cpsr));
  return _cpsr;
}

static inline void set_cpsr(unsigned long _cpsr) {
  __asm__ __volatile__ ("msr cpsr_all, %0\n\t" : : "r" (_cpsr));
}

static inline void set_spsr(unsigned long _spsr) {
  __asm__ __volatile__ ("msr spsr_all, %0\n\t" : : "r" (_spsr));
}

#define hw_cli() { \
  unsigned long _tmp_cpsr = get_cpsr (); \
  set_cpsr (_tmp_cpsr | PSR_I); \
}

#define hw_sti() { \
  unsigned long _tmp_cpsr = get_cpsr (); \
  set_cpsr (_tmp_cpsr & ~PSR_I); \
}

#define hw_restore_flags(flags) \
  set_cpsr (flags)

#define hw_save_flags(flags) \
  flags = get_cpsr ()

#define hw_save_flags_and_cli(flags) { \
  hw_save_flags(flags); \
  hw_cli(); \
}

static inline int hw_are_interrupts_enabled (void) {
  unsigned int flags = get_cpsr();
  return ((flags & PSR_I)? 0: 1);
}

#ifdef CONFIG_BOARD_INTEGRATORCP
#define hw_flush_cache() \
  __asm__ __volatile__ ("mov r0, #0\n\t" \
                        "mcr p15, 0, r0, c7, c7, 0\n\t" :::"r0")

#define hw_flush_tlb() \
  __asm__ __volatile__ ("mov r0, #0\n\t" \
                        "mcr p15, 0, r0, c8, c7, 0\n\t" :::"r0")

#endif

extern void context_switch (void *new_thread, void *current_thread);

#define CALL_SIGNAL_HANDLER(hdl, signal, sinfo, void_p)

#define CALL_CANCEL_HANDLER(hdl, arg)

#endif

#endif
