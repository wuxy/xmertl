/*
 * $FILE: stdlib.c
 *
 * Standard functions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
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

#include <config.h>
#include <stdlib.h>
#include <syscalls.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

typedef void (*atexit_func_t) (void);

static atexit_func_t atexit_func [ATEXIT_MAX] = {[0 ... ATEXIT_MAX - 1] = 0};
static int atexit_nr = 0;

//--------//
// atexit //
//--------//

int atexit (void (*function)(void)) {
  if (atexit_nr < ATEXIT_MAX) {
    atexit_func [atexit_nr ++] = function;
    return 0;
  }
  return -1;
}

//-------//
// _exit //
//-------//

void _exit (int status) {
  int res;
  __syscall1(status, exit_nr, res);
}

//------//
// exit //
//------//

void exit (int status) {
  int n;
  for (n = atexit_nr - 1; n >= 0; n--)
    (atexit_func [n]) ();
  _exit (status);
}

//-------//
// abort //
//-------//

void abort (void) {
  _exit (-1);
}

//---------//
// strtoul //
//---------//

unsigned long strtoul (const char *ptr, char **endptr, int base) {
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

//--------//
// strtol //
//--------//

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

int atoi(const char* s) {
  long int v=0;
  int sign=1;
  while ( *s == ' '  ||  (unsigned int)(*s - 9) < 5u)
	  s++;

  switch (*s) {
  case '-':
	  sign=-1;
  case '+':
	  ++s;
  }
  while ((unsigned int) (*s - '0') < 10u){
    v=v * 10 + *s - '0'; 
    ++s;
  }

  if (sign==-1)
	  return -v;
  return v;
}

#include <kernel_inc/tlsf.h>
#include <pthread.h>

static pthread_mutex_t alloc_mutex = PTHREAD_MUTEX_INITIALIZER;
extern char *_dyn_mem_pool;

//--------//
// calloc //
//--------//

void *calloc(size_t nmemb, size_t size) {
  void *ptr;
  
  pthread_mutex_lock (&alloc_mutex);
  ptr = calloc_ex (nmemb, size, _dyn_mem_pool);
  pthread_mutex_unlock (&alloc_mutex);
 
  return ptr;
}

//--------//
// malloc //
//--------//

void *malloc(size_t size) {
  void *ptr;
 
  if (!size) size = 32;
  pthread_mutex_lock (&alloc_mutex);
  ptr = malloc_ex (size, _dyn_mem_pool);

  if (ptr)
    memset ((char *)ptr, 0, size);

  pthread_mutex_unlock (&alloc_mutex);

  return ptr;
}

//------//
// free //
//------//

void free(void *ptr) {
  pthread_mutex_lock (&alloc_mutex);
  if (ptr)
    free_ex (ptr, _dyn_mem_pool);
  pthread_mutex_unlock (&alloc_mutex);
}

//---------//
// realloc //
//---------//

void *realloc(void *ptr, size_t size) {
  void *ptr_r;
 
  pthread_mutex_lock (&alloc_mutex);
  ptr_r = realloc_ex (ptr, size, _dyn_mem_pool);
  pthread_mutex_unlock (&alloc_mutex);
 
  return ptr_r;
}
