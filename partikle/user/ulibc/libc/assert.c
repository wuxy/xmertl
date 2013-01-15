/*
 * $FILE: assert.c
 *
 * Assertion facilities
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
 *
  * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//--------------//
// _assert_fail //
//--------------//

#define ASSERT_SZ 512
void _assert_fail (const char *assert, const char *file, unsigned int line, const char *func)
{
  unsigned int alen=strlen(assert);
  unsigned int flen=strlen(file);
  unsigned int fulen=func?strlen(func):0;
  char buf[ASSERT_SZ];
  
  //printf("- this is _assert_fail(tm) failing\n\n",);
  
  if ((alen+flen+fulen+25) > ASSERT_SZ){
	 printf("assert string too long (%d > %d)\n",
	 	(alen+flen+fulen+25), ASSERT_SZ);
	 return;
  }

  if (buf) {
    *buf=0;
    if (file){
	    strncat(buf, file, flen);
	    strncat(buf, ":", 2);
    }
    snprintf (buf , 9, "%d: ", line);
    if (func){
	   strncat(buf, func, fulen);
	   strncat(buf, ": ", 2);
    }
    
    strncat(buf, "Assertion `", 11);
    strncat(buf, assert, alen);
    strncat(buf, "' failed.\n", 11);
    printf("%s", buf);
  }
  abort();
}

