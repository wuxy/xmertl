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

#ifndef _STDARG_H_
#define _STDARG_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <stddef.h>

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

struct arg_scanf {
  void *data;
  int (*getch)(void*);
  int (*putch)(int,void*);
};

struct arg_printf {
  void *data;
  int (*put)(void*,size_t,void*);
};

extern int vprintf(const char *, va_list);
extern int vsscanf(const char *, const char *, va_list);
extern int vsnprintf(char *, size_t, const char *, va_list);
extern int vsprintf(char *, const char *, va_list);
extern int vfprintf(int fd, const char *format, va_list);
#endif
