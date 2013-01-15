/*
 * $FILE: bitmap.h
 *
 * Functions to work with bitmaps of up to unsigned long bits
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

#ifndef _BITMAP_H_
#define _BITMAP_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <stddef.h>
#include <bitop.h>

typedef unsigned long bitmap_t;

//-------------//
// clearbitmap //
//-------------//

static inline bitmap_t clearbitmap(void) {
  return 0;
}

//-----------//
// setbitmap //
//-----------//

static inline bitmap_t setbitmap(void) {
  return (~0);
}

//--------//
// setbit //
//--------//

static inline bitmap_t setbit(bitmap_t bitmap, int bit) {
  return (bitmap | (1 << bit));
}

//----------//
// clearbit //
//----------//

static inline bitmap_t clearbit(bitmap_t bitmap, int bit) {
  return (bitmap & ~(1 << bit));
}

//----------//
// isbitset //
//----------//

static inline int isbitset(bitmap_t bitmap, int bit) {
  return (bitmap & (1 << bit));
}

#endif
