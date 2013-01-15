/*
 * $FILE: stdarg.h
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

#ifndef _ULIBC_STDARG_H_
#define _ULIBC_STDARG_H_

#include <stdio.h>

#define __va_size(type) ((sizeof(type)+3) & ~0x3)

#ifndef _VA_LIST_
#define _VA_LIST_
typedef char *  va_list;
#endif

#define va_start(pvar, lastarg)                 \
        ((pvar) = (char*)(void*)&(lastarg) + __va_size(lastarg))
#define va_end(pvar)
#define va_arg(pvar,type)                       \
        ((pvar) += __va_size(type),             \
         *((type *)((pvar) - __va_size(type))))

extern int vprintf(const char *, va_list);
extern int vsscanf(const char *, const char *, va_list);
extern int vfscanf(FILE *, const char *, va_list);
extern int vsnprintf(char* str, size_t size, const char *format, 
		     va_list arg_ptr);
extern int vsprintf(char *dest,const char *format, 
		    va_list arg_ptr);
extern int vfprintf(int fd, const char *format, 
		    va_list arg_ptr);

#endif
