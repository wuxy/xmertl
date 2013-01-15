/*
 * $FILE: linkage.h
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

#ifndef _LINKAGE_H_
#define _LINKAGE_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#ifndef _ASSEMBLY_

#ifdef i386
#define asmlinkage __attribute__((regparm(0)))
#else
#define asmlinkage
#endif

#if defined(i386)
#define ALIGN2WORD ".align 4\n\t"
#elif defined(arm)
#define ALIGN2WORD ".align 1\n\t"
#endif

#endif

#endif
