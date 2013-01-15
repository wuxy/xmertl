/*
 * $FILE: stdio.c
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

#include <stdarg.h>
#include <stdio.h>
#include <koutput.h>

int printf(const char *format, ...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vprintf(format, arg_ptr);
  va_end(arg_ptr);
  return n;
}

int putchar(int c) {
  print_kernel ((char *)&c, 1);
  return (int) c;
}

int sscanf(const char *str, const char *format, ...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vsscanf(str,format,arg_ptr);
  va_end (arg_ptr);
  return n;
}

int snprintf(char *dest, size_t size, const char *format, ...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vsnprintf(dest, size, format, arg_ptr);
  va_end(arg_ptr);
  return n;
}

int fprintf(int fd, const char *format, ...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vfprintf(fd, format, arg_ptr);
  va_end(arg_ptr);
  return n;
}

int sprintf(char *dest, const char *format, ...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vsprintf(dest, format, arg_ptr);
  va_end(arg_ptr);
  return n;
}
