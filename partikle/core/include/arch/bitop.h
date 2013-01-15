/*
 * $FILE: bitop.h
 *
 * Some basic bit operations
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

#ifndef _ARCH_BITOP_H_
#define _ARCH_BITOP_H_

/*#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif*/

#define ARCH_HAS_FFS

//------//
// _ffs //
//------//

static __inline__ int _ffs (int x) {
  int r;

  __asm__ __volatile__ ("bsfl %1,%0\n\t"
			"jnz 1f\n\t"
			"movl $-1,%0\n"
			"1:" : "=r" (r) : "g" (x));
  return r;
}

#define ARCH_HAS_FLS

//------//
// _fls //
//------//

static __inline__ int _fls (int x) {
  int r;

  __asm__ __volatile__ ("bsrl %1,%0\n\t"
			"jnz 1f\n\t"
			"movl $-1,%0\n"
			"1:" : "=r" (r) : "g" (x));
  return r;
}

#define ARCH_HAS_SET_BIT

//----------//
// _set_bit //
//----------//

static inline void _set_bit(int nr, volatile unsigned long * addr) {
  __asm__ __volatile__ ("btsl %1,%0"
			:"=m" ((*(volatile long *) addr))
			:"Ir" (nr));
}

#define ARCH_HAS_CLEAR_BIT

//------------//
// _clear_bit //
//------------//

static inline void _clear_bit(int nr, volatile unsigned long * addr) {
  __asm__ __volatile__ ("btrl %1,%0"
			:"=m" ((*(volatile long *) addr))
			:"Ir" (nr));
}

#endif
