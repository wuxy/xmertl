/*
 * $FILE: ctype.h
 *
 * Character types
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

#ifndef _CTYPE_H_
#define _CTYPE_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

static inline int isdigit(int ch) {
  return (unsigned int)(ch - '0') < 10u;
}

static inline int isspace(int ch) {
  return (unsigned int)(ch - 9) < 5u  ||  ch == ' ';
}

static inline int isxdigit(int ch) {
  return (unsigned int)(ch - '0') < 10u  ||
    (unsigned int)((ch | 0x20) - 'a') <  6u;
}


static inline int isalnum (int ch) {
  return (unsigned int)((ch | 0x20) - 'a') < 26u  ||
    (unsigned int)(ch - '0') < 10u;
}

static inline int isinf(double d) {
  union {
    unsigned long long l;
    double d;
  } u;
  u.d=d;
  return (u.l==0x7FF0000000000000ll?1:u.l==0xFFF0000000000000ll?-1:0);
}

static inline int isnan(double d) {
  union {
    unsigned long long l;
    double d;
  } u;
  u.d=d;
  return (u.l==0x7FF8000000000000ll || u.l==0x7FF0000000000000ll || u.l==0xfff8000000000000ll);
}

#endif
