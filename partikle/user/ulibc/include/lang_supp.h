/*
 * $FILE: lang_supp.h
 *
 * Some language support stuffs
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

#ifndef _ULIBC_LANG_SUPP_H_
#define _ULIBC_LANG_SUPP_H_

#include <kernel_inc/compiler.h>
#include <sys/types.h>

BEGIN_DECLS
struct dl_phdr_info {
  unsigned long dlpi_addr;
  const char *dlpi_name;
  const unsigned long *dlpi_phdr;
  unsigned short dlpi_phnum;
};

extern int __cxa_atexit(void (*f)(void *), void *p, void *d);
extern void __cxa_finalize(void *d);
extern int dl_iterate_phdr(int (*callback) (struct dl_phdr_info *info,
					    size_t size, void *data), void *data);

END_DECLS

#endif
