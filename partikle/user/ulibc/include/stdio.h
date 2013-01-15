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

#ifndef _ULIBC_STDIO_H_
#define _ULIBC_STDIO_H_
#include <sys/types.h>
#include <stddef.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

#define EOF (-1)
#define FILE_BUFFER_SIZE 256

typedef struct file_struct {
  int fd;
  char buffer [FILE_BUFFER_SIZE];
  int bufhead, buftail, bufsize;
} FILE;

#include <stdarg.h>

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

extern int printf(const char *, ...);
extern int putchar(int c);
extern int putc(int c, FILE *s);
extern int sscanf(const char *str, const char *format, ...);
extern void *memmove(void *, const void *, size_t);
extern int scanf(const char *format, ...);
extern int getchar(void);
extern int getc(FILE *stream);
extern int fgetc(FILE *stream);
extern int ungetc(int c, FILE *stream);
extern char * fgets (char *b, int n, FILE *stream);
extern int fputs (const char *s, FILE *stream);
extern void  perror (const char* s);
extern int sprintf (char *str, const char *format, ...);
extern int snprintf (char *str, size_t, const char *format, ...);
extern int fprintf (FILE *s, const char *format, ...);

END_DECLS

#endif
