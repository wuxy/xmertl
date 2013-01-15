/*
 * $FILE: setup.c
 *
 * Setting up and starting up the kernel
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
#include <processor.h>
#include <linkage.h>
#include <stdio.h>
#include <sysmemory.h>
#include <sched.h>
#include <irqs.h>
#include <syscalls.h>
#include <stdlib.h>
#include <hwtimers.h>
#include <time.h>
#include <user.h>
#include <pthread.h>
#include <signal.h>
#include <dev.h>

// Definitions

extern unsigned long _spartikle[], _epartikle[], _stext[], _etext[], _sdata[], _edata[],
  _srodata[], _erodata[], _sbss[], _ebss[];
extern unsigned long _sapp[], _eapp[], _stext_app[], _etext_app[], 
  _sdata_app[], _edata_app[], _srodata_app[], _erodata_app[], _sbss_app[], _ebss_app[];

pthread_t mainapp_thread;

// the kernel starts the execution of the user's code in user_entry_point
extern int user_entry_point (void);

// The memory pool used by the allocator
unsigned char *memory_pool;

// The cpu frequency
hwtime_t cpu_hz;

//--------------------//
// create_main_thread //
//--------------------//

// This function is called before the first user function
static void *create_main_thread (void *main) {
  // user_jmp never should return
  exit_sys (user_jmp (main));

  return (void *) 0;
}

//--------------//
// setup_kernel //
//--------------//

// First kernel function

// This is the first C function called
// after booting the kernel
int setup_kernel (void) {
  unsigned long free_memory, cpu_khz;
  //pthread_t mainapp_thread;
  
  if (setup_cpu ()) return -1;

  // Calculating the CPU frequency
  cpu_khz = get_cpu_khz ();
  cpu_hz = (hwtime_t)cpu_khz * 1000;

  // Initialise non /dev/ devices
  links ();

  printf (">> PaRTiKle Core <<\n");
  printf ("Detected %u.%03u MHz processor.\n",
	  cpu_khz / 1000, cpu_khz % 1000);

  // Creating the free memory map
  if ((free_memory = init_sysmem ()) == -1) {
    printf ("\nFree system memory unknown\n");
    return -1;
  }
  
  printf ("\nFree system memory %u Kb\n", (unsigned long) free_memory/1024);
  printf ("\nPaRTiKle (%u Kb ",
	  ((unsigned long)_epartikle -
	   (unsigned long)_spartikle)/1024);
  printf ("[.text=%u ",
             ((unsigned long)_etext - (unsigned long)_stext)/1024);
  printf (".data=%u ",
             ((unsigned long)_edata - (unsigned long)_sdata)/1024);
  printf (".rodata=%u ",
             ((unsigned long)_erodata -
              (unsigned long)_srodata)/1024);
  printf (".bss=%u] Kb)\n",
	  ((unsigned long)_ebss - (unsigned long)_sbss)/1024);
  printf ("App. (%u Kb ",
	  ((unsigned long)_eapp -
	   (unsigned long)_sapp)/1024);
  printf ("[.text=%u ",
             ((unsigned long)_etext_app - (unsigned long)_stext_app)/1024);
  printf (".data=%u ",
             ((unsigned long)_edata_app - (unsigned long)_sdata_app)/1024);
  printf (".rodata=%u ",
             ((unsigned long)_erodata_app -
              (unsigned long)_srodata_app)/1024);
  printf (".bss=%u] Kb)\n",
	  ((unsigned long)_ebss_app - (unsigned long)_sbss_app)/1024);

  configure_stack (STACK_SIZE);
  
  // Initialise /dev/ devices
  if (init_devs () < 0)
    return -1;
  
  // Initialisating the scheduler
  init_sched ();

  // Bringing up the signal subsystem
  if (init_signals ()) return -1;

  // Starting up system's timers and clocks
  if (init_time ()) return -1;
  
  // Current thread is our idle thread
  create_idle ();

  // For some architectures, creating the application's memory map is
  // compulsory
  create_app_map ();

  // Enabling interrupts
  hw_sti ();

  printf ("\nJumping to the user's code\n\n");
  
  //  Calling the first user function
  pthread_create_sys (&mainapp_thread, NULL, (void *(*)(void *))
		      create_main_thread, (void *) user_entry_point);
  
  // Idle
  current_thread -> sigmask.sig = ~0;
  while (1) {
    // Sometimes there are detached, finished threads which have to be
    // removed 
    IDLE;
  }

  return 0;
}

//----------//
// exit_sys //
//----------//

extern void _exit (int);
asmlinkage void exit_sys (int status) {
  // Disabling interrupts
  hw_cli();
  printf ("\nSystem exit status: %d.\n\n", status);
  _exit (status);
}
