/*
 * $FILE: tsc.c
 *
 * TSC driver
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

#include <linux_inc.h>
#include <hwtimer.h>

// Definitions

static long long hw_hz = 0; 
long long cpu_hz = 0;
static unsigned long calibrate_tsc (void);
static unsigned int get_cpu_khz (unsigned long tsc_quotient);

//----------//
// init_tsc //
//----------//

static int init_tsc (void) {
  unsigned long tsc_quotient = calibrate_tsc ();
  unsigned int cpu_khz = get_cpu_khz (tsc_quotient);

  printk("<XtratuM> Detected %u.%03u MHz processor.\n",
	 cpu_khz / 1000, cpu_khz % 1000);
  
  hw_hz = cpu_hz = (unsigned long long)cpu_khz * 1000;
  return 0;
}

//----------//
// read_tsc //
//----------//

#define rdtscll(val) \
  __asm__ __volatile__("rdtsc" : "=A" (val))

static hwtime_t read_tsc (void) {
  hwtime_t tsc_time;
  rdtscll (tsc_time);
  return tsc_time;
}

//---------------//
// read_tsc_nsec //
//---------------//

static duration_t read_tsc_nsec (void) {
  hwtime_t tsc_time, s;
  rdtscll (tsc_time);
  s = tsc_time / hw_hz;
  return s * NSECS_PER_SEC + 
    ((tsc_time - s * hw_hz) * NSECS_PER_SEC) / hw_hz;
}

//---------------------//
// hwtime2duration_tsc //
//---------------------//

static duration_t hwtime2duration_tsc (hwtime_t hwtime) {
  hwtime_t s;
  s = hwtime/hw_hz;
  return s*NSECS_PER_SEC+((hwtime-s*hw_hz)*NSECS_PER_SEC)/hw_hz;
}

//---------------------//
// duration2hwtime_tsc //
//---------------------//

static hwtime_t duration2hwtime_tsc (duration_t d) {
  hwtime_t s;
  s = d/NSECS_PER_SEC;
  return s*hw_hz +((d-s*NSECS_PER_SEC)*hw_hz)/NSECS_PER_SEC;
}

//------------//
// getres_tsc //
//------------//

static duration_t getres_tsc (void) {
  return 1/hwtime2duration_tsc (hw_hz);
}

//--------------//
// shutdown_tsc //
//--------------//

static void shutdown_tsc (void) {
}

//---------------//
// calibrate_tsc //
//---------------//

#define CALIBRATE_LATCH (5 * LATCH)
#define CALIBRATE_TIME (5 * 1000020/HZ)

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

static unsigned int get_cpu_khz (unsigned long tsc_quotient) {
  unsigned int cpu_khz;
  unsigned long eax=0, edx=1000;
  __asm__("divl %2"
	  :"=a" (cpu_khz), "=d" (edx)
	  :"r" (tsc_quotient),
	  "0" (eax), "1" (edx));
  return cpu_khz;
}

sysclock_t sysclock = {
  init_clock: init_tsc,
  gettime_hwt: read_tsc,
  gettime_nsec: read_tsc_nsec,
  hwtime2duration: hwtime2duration_tsc,
  duration2hwtime: duration2hwtime_tsc,
  getres: getres_tsc,
  shutdown_clock: shutdown_tsc,
};

