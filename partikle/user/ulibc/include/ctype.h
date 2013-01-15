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

#ifndef _ULIBC_CTYPE_H_
#define _ULIBC_CTYPE_H_

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

static inline int isupper(int ch) {
  return ('A' <= ch && ch <= 'Z');
}

static inline int islower(int ch) {
  return ('a' <= ch && ch <= 'z');
}

static inline int isalpha(int ch) {
  return (unsigned int)((ch | 0x20) - 'a') < 26u;
}

static inline int isblank(int ch) {
    return ch == ' '  ||  ch == '\t';
}

static inline int iscntrl(int ch) {
  return (unsigned int)ch < 32u  ||  ch == 127;
}

static inline int isgraph(int ch) {
  return (unsigned int)(ch - '!') < 127u - '!';
}

static inline int isprint (int ch) {
  ch&=0x7f;
  return (ch>=32 && ch<127);
}

static inline int ispunct (int ch) 
{
    return isprint (ch)  &&  !isalnum (ch)  &&  !isspace (ch);
}

static inline int toupper(int ch) {
  if ( (unsigned int)(ch - 'a') < 26u )
    ch += 'A' - 'a';
  return ch;
}

static inline int tolower(int ch) {
  if ( (unsigned int)(ch - 'A') < 26u )
    ch += 'a' - 'A';
  return ch;
}

#endif
