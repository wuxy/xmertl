/*
 * $FILE: string.h
 *
 * String operations
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

#ifndef _ULIBC_STRING_H_
#define _ULIBC_STRING_H_

#include <stddef.h>
#include <sys/types.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

extern int memcmp(const void *, const void *, size_t);
extern void *memcpy(void *, const void *, size_t);
extern void *memset(void *, int, size_t);
extern char *strcat(char *, const char *);
extern char *strchr(const char *, int);
extern int strcmp(const char *, const char *);
extern int strncmp(const char *, const char *, size_t);
extern char *strcpy(char *, const char *);
extern char *strncpy(char *dest, const char *src, size_t n);
extern size_t strlen(const char *);
extern char *strrchr(const char *, int);
extern char *strstr(const char *, const char *);
extern char *strerror(int errnum);
extern void *memccpy(void *dst, const void *src, int c, size_t count);
extern void* memchr(const void *s, int c, size_t n);
extern void *memmove(void *dst, const void *src, size_t count);
extern char *strdup(const char *s);
extern char *strncat(char *s, const char *t, size_t n);
extern size_t strspn(const char *s, const char *accept);
extern size_t strcspn(const char *s, const char *reject);
extern char *strtok_r(char *s, const char *delim, char **ptrptr);
extern char *strtok(char *s, const char *delim);
END_DECLS

#endif
