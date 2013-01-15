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

#include <pthread.h>
#include <sched.h>
#include <stack.h>
#include <tlsf.h>
#include <time.h>
#include <arch/processor.h>

//-----------------//
// configure_stack //
//-----------------//

void configure_stack (unsigned long size) {
}

//---------//
// startup //
//---------//
extern pthread_t mainapp_thread;
// First function executed when a new thread is created
static int startup (void *(*start_routine) (void *), void *args) {
  void *retval;
  
  current_thread -> starting_time = monotonic_clock->gettime_nsec();
  // Initialising FPU
  __asm__ __volatile__ ("finit\n\t" ::);
  hw_sti ();
  retval = (*start_routine) (args);

  pthread_exit_sys (retval);
  // This point will (or should) never be reached
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

//-------------//
// alloc_stack //
//-------------//

int alloc_stack (stack_t *stack) {
  if (!(stack -> stack_bottom = 
	(unsigned long *) malloc_ex (stack -> stack_size, memory_pool)))
    return -1;
  
  return 0;
}

//---------------//
// dealloc_stack //
//---------------//

void dealloc_stack (stack_t *stack) {
  if (stack -> stack_bottom)
    free_ex (stack -> stack_bottom, memory_pool);
}
