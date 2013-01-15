/*
 * $FILE: processor.c
 *
 * Processor arch-dependient functions
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
#include <hwtimers.h>
#include <irqs.h>
#include <arch/io.h>
#include <stdio.h>
#include <arch/multiboot.h>
#include <arch/processor.h>

unsigned int cpu_fflags;
tss_t tss;
multiboot_info_t *mbi;

//-----------//
// setup_tss //
//-----------//

/*
 * TODO: check if this function has been correctly written ;-)
 */

static void setup_tss (int segment) {
  unsigned long _base = (unsigned long) &tss, _limit = sizeof (tss_t) - 1;
  tss.ss0 = KERNEL_DS;
  tss.esp0 = 0;
  tss.io_bit_map_offset = sizeof (tss_t);
  gdt [segment/8].high = ((_base << 16) & 0xFFFF0000) |
    (0xFFFF & _limit);
  gdt [segment/8].low = 0x8900 | ((_base >> 16) & 0xFF) |
    (_base & 0xFF000000) | (_limit & 0xF0000);
  set_tr (segment);
}

//---------------//
// calibrate_tsc //
//---------------//

#define CLOCK_TICK_RATE 1193180
#define HZ 100
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)
#define CALIBRATE_LATCH (5 * LATCH)
#define CALIBRATE_TIME  (5 * 1000020/HZ)

static unsigned long calibrate_tsc(void) {
  /* Set the Gate high, disable speaker */
  outb((inb(0x61) & ~0x02) | 0x01, 0x61);
  
  /*
   * Now let's take care of CTC channel 2
   *
   * Set the Gate high, program CTC channel 2 for mode 0,
   * (interrupt on terminal count mode), binary count,
   * load 5 * LATCH count, (LSB and MSB) to begin countdown.
   *
   * Some devices need a delay here.
   */
  outb(0xb0, 0x43);                       /* binary, mode 0, LSB/MSB, Ch 2 */
  outb_p(CALIBRATE_LATCH & 0xff, 0x42);   /* LSB of count */
  outb_p(CALIBRATE_LATCH >> 8, 0x42);     /* MSB of count */ 
  
  {
    unsigned long startlow, starthigh;
    unsigned long endlow, endhigh;
    unsigned long count = 0;
    
    rdtsc(startlow,starthigh);
    do {
      count++;
    } while ((inb_p(0x61) & 0x20) == 0);
    rdtsc(endlow,endhigh);
    
    /* Error: ECTCNEVERSET */
    if (count <= 1)
      goto bad_ctc;
    
    /* 64-bit subtract - gcc just messes up with long longs */
    __asm__("subl %2,%0\n\t"
	    "sbbl %3,%1"
	    :"=a" (endlow), "=d" (endhigh)
	    :"g" (startlow), "g" (starthigh),
	    "0" (endlow), "1" (endhigh));
    
    /* Error: ECPUTOOFAST */
    if (endhigh)
      goto bad_ctc;
    
    /* Error: ECPUTOOSLOW */
    if (endlow <= CALIBRATE_TIME)
      goto bad_ctc;
    
    __asm__("divl %2"
	    :"=a" (endlow), "=d" (endhigh)
	    :"r" (endlow), "0" (0), "1" (CALIBRATE_TIME));
    
    return endlow;
  }
  
  /*
   * The CTC wasn't reliable: we got a hit on the very first read,
   * or the CPU was so fast/slow that the quotient wouldn't fit in
   * 32 bits..
   */
 bad_ctc:
  return 0;
}

//-------------//
// get_cpu_khz //
//-------------//

unsigned int get_cpu_khz (void){
  unsigned int cpu_khz;
  unsigned long tsc_quotient, eax=0, edx=1000;

  tsc_quotient = calibrate_tsc ();

  __asm__("divl %2"
	  :"=a" (cpu_khz), "=d" (edx)
	  :"r" (tsc_quotient),
	  "0" (eax), "1" (edx));
  return cpu_khz;
}

//----------//
// iopl_sys //
//----------//

int iopl_sys (int val) {
  return 0;
}

//-----------//
// setup_cpu //
//-----------//

int setup_cpu (void) {
  setup_tss (TSS_SEGMENT);
  init_irqs_traps ();
  return 0;
}
