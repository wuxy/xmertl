/*
 * $FILE: note.h
 *
 * ELF's NOTE section definition
 *
 * $VERSION: 1.0
 *
 * Miguel Masmano <mimastel@doctor.upv.es>
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

#ifndef _NOTE_H_
#define _NOTE_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <config.h>

#define ASM_ELF_NOTE_BEGIN(name, flags, vendor, type)			      \
	.section name, flags;						      \
	.balign 4;							      \
	.long 1f - 0f;		/* name length */			      \
	.long 3f - 2f;		/* data length */			      \
	.long type;		/* note type */				      \
0:	.asciz vendor;		/* vendor name */			      \
1:	.balign 4;							      \
2:

#define ASM_ELF_NOTE_END						      \
3:	.balign 4;		/* pad out section */			      \
	.previous

#endif
