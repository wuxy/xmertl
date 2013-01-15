/*
 * $FILE: stdlib.c
 *
 * Standard library definitions
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

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

unsigned long strtoul(const char *ptr, char **endptr, int base) {
  int neg = 0, overflow = 0;
  unsigned long int v = 0;
  const char* orig;
  const char* nptr = ptr;
  
  while(isspace(*nptr)) ++nptr;
  
  if (*nptr == '-') { neg=1; nptr++; }
  else if (*nptr == '+') ++nptr;
  orig = nptr;
  if (base == 16 && nptr[0] == '0') goto skip0x;
  if (base) {
    register unsigned int b = base-2;
    if (b>34) { return 0; }
  } else {
    if (*nptr == '0') {
      base=8;
    skip0x:
      if ((nptr[1] == 'x'||nptr[1] == 'X') && isxdigit(nptr[2])) {
	nptr += 2;
	base = 16;
      }
    } else
      base=10;
  }
  while(*nptr) {
    register unsigned char c = *nptr;
    c = (c>='a'?c-'a'+10:c>='A'?c-'A'+10:c<='9'?c-'0':0xff);
    if (c>=base) break;	/* out of base */
    {
      register unsigned long x = (v&0xff)*base+c;
      register unsigned long w = (v>>8)*base+(x>>8);
      if (w>(ULONG_MAX>>8)) overflow = 1;
      v = (w<<8)+(x&0xff);
    }
    ++nptr;
  }
  if (nptr == orig) {		/* no conversion done */
    nptr = ptr;
    v = 0;
  }
  if (endptr) *endptr = (char *)nptr;
  if (overflow) {
    return ULONG_MAX;
  }
  return (neg?-v:v);
}

#define ABS_LONG_MIN 2147483648UL

long int strtol(const char *nptr, char **endptr, int base) {
  int neg=0;
  unsigned long int v;
  const char*orig=nptr;

  while(isspace(*nptr)) nptr++;

  if (*nptr == '-' && isalnum(nptr[1])) { neg=-1; ++nptr; }
  v=strtoul(nptr,endptr,base);
  if (endptr && *endptr==nptr) *endptr=(char *)orig;
  if (v>=ABS_LONG_MIN) {
    if (v==ABS_LONG_MIN && neg) {
      return v;
    }
    return (neg?LONG_MIN:LONG_MAX);
  }
  return (neg?-v:v);
}

