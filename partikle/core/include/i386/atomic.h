/*
 * $FILE: atomic.h
 *
 * Atomic operation, useful to implement mutex and so on
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

#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

typedef struct {
  volatile int c;
} atomic_t;

#define ATOMIC_INIT(i) {(i)}

#define atomic_read(v) ((v)->c)
#define atomic_set(v,i) (((v)->c) = (i))

static inline void atomic_add (int i, atomic_t *v) {
  __asm__ __volatile__ ("addl %1,%0\n\t"
			:  "=m" (v -> c)
			: "ir" (i), "m" (v -> c));
}

static inline void atomic_inc (atomic_t *v) {
  __asm__ __volatile__ ("incl %0\n\t"
		        :"=m" (v->c)
		        :"m" (v->c));
}

static inline int atomic_inc_and_test (atomic_t *v) {
  unsigned char c;
  
  __asm__ __volatile__ ("incl %0\n\t"
		        "sete %1\n\t"
		        :"=m" (v->c), "=qm" (c)
		        :"m" (v->c) : "memory");
  return c != 0;
}

static inline void atomic_dec (atomic_t *v) {
  __asm__ __volatile__ ("decl %0\n\t"
			:"=m" (v->c)
			:"m" (v->c));
}

static inline int atomic_dec_and_test (atomic_t *v) {
  unsigned char c;
  
  __asm__ __volatile__ ("decl %0\n\t"
			"sete %1\n\t"
			:"=m" (v->c), "=qm" (c)
			:"m" (v->c) : "memory");
  return c != 0;
}

#endif
