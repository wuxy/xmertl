/*
 * $FILE: stdio.h
 *
 * Standard buffered input/output
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

#ifndef _STDIO_H_
#define _STDIO_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <stddef.h>
#include <stdarg.h>

#define EOF (-1)

extern int printf(const char *, ...);
extern int putchar(int c);
extern int sscanf(const char *str, const char *format, ...);
extern void *memmove(void *, const void *, size_t);
extern int snprintf(char *, size_t, const char *, ...);
extern int sprintf(char *, const char *, ...);
extern int fprintf(int, const char *, ...);

#endif
