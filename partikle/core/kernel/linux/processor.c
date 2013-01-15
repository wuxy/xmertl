/*
 * $FILE: processor.c
 *
 * Processor-related functions
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

#include <hwtimers.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arch/linux_syscalls.h>
#include <arch/processor.h>
#include <irqs.h>

//-------------//
// get_cpu_khz //
//-------------//

hwtime_t get_cpu_khz (void) {
  int procfd;
  float cpu_mhz = 0;
  
  /* Get the processor frequency.  */
  procfd = linux_open ("/proc/cpuinfo", O_RDONLY, 0);
  if (procfd != -1) {
    char buf[2000];
    ssize_t n;

    if ((n = linux_read (procfd, buf, sizeof (buf))) > 0) {
      char *cpuhz;

      buf[sizeof (buf) - 1] = '\0';
      cpuhz = (char *) strstr (buf, "cpu MHz");
      if (cpuhz != NULL) {
        cpuhz += 7;

        while (!isdigit (*cpuhz))
          ++cpuhz;
		sscanf (cpuhz, "%f", &cpu_mhz);
      }
    }
    
    linux_close (procfd);
    
    return (hwtime_t) (cpu_mhz * 1000);
  }

  return 0;
}

//----------//
// iopl_sys //
//----------//

int iopl_sys (int val) {
  //  printf ("---------> IOPL: linux_iopl(%d)= %d\n", val, linux_iopl(val));
  return linux_iopl (val);
}

int setup_cpu (void) {
	init_arch_irqs();
	return 0;
}
