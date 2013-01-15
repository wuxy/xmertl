/*
 * $FILE: processor.c
 *
 * Processor arch-dependient functions
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
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
#include <irqs.h>
#include <syscalls.h>
#include <stdio.h>
#include <string.h>

#include "system.h"
#include "lib.h"
#include "mem.h"

unsigned long
	_spartikle=(ulong)btext, 
	_epartikle=(ulong)ebss,
	_stext=(ulong)btext,
	_etext=(ulong)etext,
	_sdata=(ulong)bdata,
	_edata=(ulong)edata,
	_srodata=(ulong)bdata,
	_erodata=(ulong)edata,
	_sbss=(ulong)bbss,
	_ebss=(ulong)ebss;

unsigned long
	_sapp,
	_eapp,
	_stext_app,
	_etext_app, 
	_sdata_app,
	_edata_app,
	_srodata_app,
	_erodata_app,
	_sbss_app,
	_ebss_app;

#define doc     if(0) rprintf

extern void setup_kernel(void);
void
maininit(){
	sysinit();
	uartinit(38400);

	doc("\nsysinit ...\n");
	doc("uartinit ...\n");

	doc("initdata ...\n");
	/* zero the .bss section */
	memset(bbss, 0, (ebss - bbss)*sizeof(ulong));
	/* copy .data section (cp ROM to RAM) */
	memcpy (bdata, etext, (edata - bdata)*sizeof(ulong));
	
	doc("memcmp ...");
	if (! memcmp(bdata, etext, (edata - bdata)*sizeof(ulong))){
		doc("ok\n");
	}else{
		doc("ko\n");
		trap();
	}
	
	doc("trapinit ...");
	if (trapinit())
		doc("ok\n");
	
        // are we missing to enable irq/fiq ?
        enableirq(); enablefiq();
        
	if(0)dumpstate();
	
	rprintf("\n");	// start on a new line
	setup_kernel();
}

/*Initialization of the ARM system*/
int
sysinit()
{
	// setup & enable the MAM
	MAMTIM = MAMTIM_CYCLES;  //0x04
	MAMCR = MAMCR_FULL;
	
	// set the interrupt controller defaults
#if defined(RAM_RUN)
	MEMMAP = MEMMAP_SRAM;                 // map interrupt vectors space into SRAM
#elif defined(ROM_RUN)
	MEMMAP = MEMMAP_FLASH;                // map interrupt vectors space into FLASH
#else
#error RUN_MODE not defined!
#endif

	//init_IRQ();

	PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL;
	
	// enable PLL
	PLLCON = PLLCON_PLLE;
	
	PLLFEED = 0x000000AA;                       // Make it happen.  These two updates
	PLLFEED = 0x00000055;                       // MUST occur in sequence.
	
	// wait for PLL lock
	while (!(PLLSTAT & PLLSTAT_LOCK))
		continue;
	
	// enable & connect PLL
	PLLCON = PLLCON_PLLE | PLLCON_PLLC;
	PLLFEED = 0x000000AA;                       
	PLLFEED = 0x00000055;                       // MUST occur in sequence.
	
	// set the peripheral bus speed
	VPBDIV = VPBDIV_VALUE;                // set the peripheral bus clock speed

	return 0;
}

//-----------//
// setup_cpu //
//-----------//
int setup_cpu (void) {
	return 0;
}

//----------//
// iopl_sys //
//----------//

int iopl_sys (int level) {
	return 0;
}

//-------------//
// get_cpu_khz //
//-------------//

unsigned long get_cpu_khz (void) {
	return CCLK / 1000;
}

void _exit (int retval){
	printf("_exit()\n");
	while (1)
		;
}

int syscall_gate (unsigned long a0, unsigned long a1, 
		  unsigned long a2, unsigned long a3, 
		  unsigned long a4, unsigned long syscallnr) {
  int ret;
  
  if (syscallnr >= NR_SYSCALLS) {
    printf ("System call (%d) unknown\n", syscallnr);
    return -1;
  }

  ret = ((int (*) (unsigned long, unsigned long, unsigned long, 
		   unsigned long, unsigned long))syscall_table [syscallnr]) 
    (a0, a1, a2, a3, a4);

  return ret;
}

int init_arch_irqs (void) {
	return 0;
}

void panic (context_t *regs, char *fmt, ...) {
	trap();
	_exit (-1);
}
