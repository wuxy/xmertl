/*
 * $FILE: stack.h
 *
 * Stack operations
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

#ifndef _STACK_H_
#define _STACK_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

typedef struct stack_struct {
  unsigned long *stack_bottom, stack_size;
} stack_t;

extern void configure_stack (unsigned long size);
extern int alloc_stack (stack_t *);
extern void dealloc_stack (stack_t *);
extern unsigned long *setup_stack (unsigned long *, 
		                   void *(*) (void *), void *);
	  

#endif
