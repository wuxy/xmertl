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

#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <config.h>
#include <irqs.h>
#include <hwtimers.h>
#include <arch/processor.h>

// Only once the next function has been called cpu_freq is valid
extern hwtime_t cpu_hz;
extern int setup_cpu (void);
extern hwtime_t get_cpu_khz (void);
extern void panic (context_t *regs, char *fmt, ...);
extern void dump_state (context_t *regs);

#endif
