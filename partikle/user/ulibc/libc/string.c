/*
 * $FILE: string.c
 *
 * String related functions
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

#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errstr.h>
#include <stdlib.h>

#define __likely(foo) __builtin_expect((foo),1)
#define __unlikely(foo) __builtin_expect((foo),0)

//--------//
// memset //
//--------//

void *memset (void *dst, int s, unsigned int count) {
  register char * a = dst;
  count++;
  while (--count)
    *a++ = s;
  return dst;
}

//--------//
// memcpy //
//--------//

void *memcpy(void* dst, const void* src, unsigned int count) {
  register char *d=dst;
  register const char *s=src;
  ++count;
  while (--count) {
    *d = *s;
    ++d; ++s;
  }
  return dst;
}

//--------//
// memcmp //
//--------//

int memcmp(const void *dst, const void *src, unsigned int count) {
  int r;
  const char *d=dst;
  const char *s=src;
  ++count;
  while (__likely(--count)) {
    if (__unlikely(r=(*d - *s)))
      return r;
    ++d;
    ++s;
  }
  return 0;
}

//--------//
// strcpy //
//--------//

char *strcpy (char *dst, const char *src) {
  char *aux = dst;
  while ((*aux++ = *src++));
  return dst;
}

//---------//
// strncpy //
//---------//

char *strncpy(char *dest, const char *src, size_t n) {
  int j;

  memset(dest,0,n);

  for (j = 0; j < n && src [j]; j ++)
    dest [j] = src [j];
  
  if (j >= n)
    dest [n - 1] = 0;

  return dest;
}

//--------//
// strcat //
//--------//

char *strcat(char* s, const char* t) {
  char *dest=s;
  s+=strlen(s);
  for (;;) {
    if (!(*s = *t)) break; ++s; ++t;
  }
  return dest;
}

//--------//
// strcmp //
//--------//

int strcmp(const char *s, const char *t) {
  char x;

  for (;;) {
    x = *s; if (x != *t) break; if (!x) break; ++s; ++t;
  }
  return ((int)(unsigned int)(unsigned char) x)
    - ((int)(unsigned int)(unsigned char) *t);
}

//---------//
// strncmp //
//---------//

int strncmp(const char *s1, const char *s2, size_t n) {
  register const unsigned char *a=(const unsigned char*)s1;
  register const unsigned char *b=(const unsigned char*)s2;
  register const unsigned char *fini=a+n;

  while (a < fini) {
    register int res = *a-*b;
    if (res) return res;
    if (!*a) return 0;
    ++a; ++b;
  }
  return 0;
}

//--------//
// strlen //
//--------//

unsigned int strlen(const char *s) {
  unsigned int i;
  if (!s) return 0;
  for (i = 0; *s; ++s) ++i;
  return i;
}

//---------//
// strrchr //
//---------//

char *strrchr(const char *t, int c) {
  char ch;
  const char *l=0;

  ch = c; 
  for (;;) {
    if (*t == ch) l=t; 
    if (!*t) return (char*)l; ++t;
  }
  
  return (char*)l;
}

//--------//
// strchr //
//--------//

char *strchr(const char *t, int c) {
  register char ch;

  ch = c;
  for (;;) {
    if (*t == ch) break; if (!*t) return 0; ++t;
  }
  return (char*)t;
}

//--------//
// strstr //
//--------//

char *strstr(const char *haystack, const char *needle) {
  size_t nl=strlen(needle);
  size_t hl=strlen(haystack);
  int i;
  if (!nl) goto found;
  if (nl>hl) return 0;
  for (i=hl-nl+1; i; --i) {
    if (*haystack==*needle && !memcmp(haystack,needle,nl))
found:
      return (char*)haystack;
    ++haystack;
  }
  return 0;
}

//---------//
// memmove //
//---------//

void *memmove(void *dst, const void *src, size_t count) {
  char *a = dst;
  const char *b = src;
  if (src!=dst)
  {
    if (src>dst)
    {
      while (count--) *a++ = *b++;
    }
    else
    {
      a+=count-1;
      b+=count-1;
      while (count--) *a-- = *b--;
    }
  }
  return dst;
}

//----------//
// strerror //
//----------//

char *strerror(int err) {
  const char *message=__sys_err_unknown;
  int n = sizeof errstr / sizeof errstr[0];

  if (n == 2)
    message=errstr[1];
  else if (0 <= err && err < n)
    message=errstr[err];

  return (char*) message;
}

//---------//
// memccpy //
//---------//

void *memccpy(void *dst, const void *src, int c, size_t count)
{
  char *a = dst;
  const char *b = src;
  while (count--)
  {
    *a++ = *b;
    if (*b==c)
    {
      return (void *)a;
    }
    b++;
  }
  return 0;
}

//--------//
// memchr //
//--------//

void* memchr(const void *s, int c, size_t n) {
  const unsigned char *pc = (unsigned char *) s;
  for (;n--;pc++)
    if (*pc == c)
      return ((void *) pc);
  return 0;
}


//--------//
// strdup //
//--------//

char *strdup(const char *s) {
  char *tmp=(char *)malloc(strlen(s)+1);
  if (!tmp) return 0;
  strcpy(tmp,s);
  return tmp;
}

//---------//
// strncat //
//---------//

char *strncat(char *s, const char *t, size_t n) {
  char *dest=s;
  register char *max;
 
  s += strlen(s);
  if ((max = s + n) == s)
    goto fini;
  for (;;) {
    if (__unlikely(!(*s = *t)))
      break;
    if (__unlikely(++s==max))
      break;
    ++t;
  }
  *s = 0;
fini:
  return dest;
}

//---------//
// strnspn //
//---------//

size_t strspn(const char *s, const char *accept) {
  size_t l=0;
  int a=1, i, al = strlen(accept);

  while((a) && (*s))
  {
    for(a=i=0; (!a) && (i<al); i++)
      if (*s == accept[i])
        a=1;
    if (a)
      l++;
    s++;
  }
  return l;
}

//---------//
// strcspn //
//---------//

size_t strcspn(const char *s, const char *reject)
{
  size_t l=0;
  int a=1, i, al = strlen(reject);

  while((a) && (*s))
  {
    for(i=0; (a) && (i<al); i++)
      if (*s == reject[i])
        a=0;
    if (a)
      l++;
    s++;
  }
  return l;
}


//----------//
// strtok_r //
//----------//

char *strtok_r(char *s, const char *delim, char **ptrptr) {
  char *tmp=0;

  if (s == 0) s = *ptrptr;
  s+=strspn(s,delim);		/* overread leading delimiter */
  if (__likely(*s)) {
    tmp=s;
    s+=strcspn(s,delim);
    if (__likely(*s))
      *s++=0;	/* not the end ? => terminate it */
  }
  *ptrptr = s;
  return tmp;
}

//--------//
// strtok //
//--------//

static char *strtok_pos;

char *strtok(char *s, const char *delim)
{
  return strtok_r(s, delim, &strtok_pos);
}
