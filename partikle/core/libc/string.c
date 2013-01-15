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

#include <string.h>
#include <ctype.h>
#include <limits.h>

void *memset (void *dst, int s, unsigned int count) {
  register char * a = dst;
  count++;
  while (--count)
    *a++ = s;
  return dst;
}

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

int memcmp(const void *dst, const void *src, unsigned int count) {
  int r;
  const char *d=dst;
  const char *s=src;
  ++count;
  while (--count) {
    if ((r=(*d - *s)))
      return r;
    ++d;
    ++s;
  }
  return 0;
}

char *strcpy (char *dst, const char *src) {
  char *aux = dst;
  while ((*aux++ = *src++));
  return dst;
}

char *strncpy (char *dst, const char *src, int n) {
  char *aux = dst;
  while ((n-- > 0) && (*aux++ = *src++));
  return dst;
}


char *strcat(char* s, const char* t) {
  char *dest=s;
  s+=strlen(s);
  for (;;) {
    if (!(*s = *t)) break; ++s; ++t;
  }
  return dest;
}


int strcmp(const char *s, const char *t) {
  char x;

  for (;;) {
    x = *s; if (x != *t) break; if (!x) break; ++s; ++t;
  }
  return ((int)(unsigned int)(unsigned char) x)
    - ((int)(unsigned int)(unsigned char) *t);
}


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

unsigned int strlen(const char *s) {
  unsigned int i;
  if (!s) return 0;
  for (i = 0; *s; ++s) ++i;
  return i;
}

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

char *strchr(const char *t, int c) {
  register char ch;

  ch = c;
  for (;;) {
    if (*t == ch) break; if (!*t) return 0; ++t;
  }
  return (char*)t;
}


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
