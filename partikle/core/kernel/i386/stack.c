/*
 * $FILE: stack.c
 *
 * Stack management functions
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

#include <config.h>
#include <pthread.h>
#include <sched.h>
#include <stack.h>
#include <string.h>
#include <time.h>
#include <tlsf.h>
#include <sysmemory.h>
#include <arch/processor.h>
#include <arch/paging.h>

//-----------------//
// configure_stack //
//-----------------//

// The stacks in the i386 arch will be mapped in the address 0xbce00000
extern unsigned long _sstack;

void configure_stack (unsigned long size) {
#ifdef CONFIG_X86_PAGING
  int pgd_e, pgt_e;
  unsigned int *pgt;
  unsigned long page, addr, 
    stack_addr = VIRT2PHYS((unsigned long) &_sstack), esp;

  for (addr = STACK_ADDR; addr < STACK_ADDR + size; 
       addr += PAGE_SIZE, stack_addr += PAGE_SIZE) {
    pgd_e = va2pgd (addr);
    pgt_e = va2pgt (addr);
    if (!pgd[pgd_e]) {
      page = alloc_phys_page ();
      pgd [pgd_e] = page | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
      pg_pgt [pgd_e] = page | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
      pgt = (unsigned int *)pgdpgt2va (PGTS_ENTRY, pgd_e);
      CLEAN_PAGE (pgt);
    }
    pgt = (unsigned int *)pgdpgt2va (PGTS_ENTRY, pgd_e);
    pgt [pgt_e] = stack_addr | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
  }
  
  hw_get_stack (esp);

  esp = (esp - (unsigned long)&_sstack) + STACK_ADDR;
 
  hw_set_stack (esp);
#endif
}

//-------------//
// alloc_stack //
//-------------//

int alloc_stack (stack_t *stack) {
#ifdef CONFIG_X86_PAGING
  if (!(stack -> stack_bottom = 
	(unsigned long *) alloc_region (STACK_ADDR, OFFSET-1, 
					stack -> stack_size)))
    return -1;
#else
  if (!(stack -> stack_bottom = 
	(unsigned long *) malloc_ex (stack -> stack_size, memory_pool)))
    return -1;
#endif

  return 0;
}

//---------------//
// dealloc_stack //
//---------------//

void dealloc_stack (stack_t *stack) {
#ifdef CONFIG_X86_PAGING
  if (stack -> stack_bottom)
    dealloc_region (stack -> stack_bottom, stack -> stack_size);
#else
  if (stack -> stack_bottom)
    free_ex (stack -> stack_bottom, memory_pool);
#endif
}

//---------//
// startup //
//---------//

// First function executed when a new thread is created
static int startup (void *(*start_routine) (void *), void *args) {
  void *retval;

  current_thread -> starting_time = monotonic_clock->gettime_nsec(); 
  // Initialising FPU
  __asm__ __volatile__ ("finit\n\t" ::);
  hw_sti ();
  retval = (*start_routine) (args);
  pthread_exit_sys (retval);

  // This point will never be reached
  return 0;
}

//-------------//
// setup_stack //
//-------------//

unsigned long *setup_stack (unsigned long *stack, 
			    void *(*start_routine) (void *), void *args) {
  // The arguments of the startup
  *--(stack) = (unsigned long) args;
  *--(stack) = (unsigned long) start_routine;
  // Dummy return address
  *--(stack) = 0;
  // First executed function
  *--(stack) = (unsigned long) startup;
  return stack;
}
