/*
 * $FILE: cbitmap.h
 *
 * Functions to work with bitmaps
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

#ifndef _CBITMAP_H_
#define _CBITMAP_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <stddef.h>
#include <string.h>
#include <bitop.h>

#define ARRAY_SIZE(bits) \
  ((bits % BITS_PER_WORD)?bits / BITS_PER_WORD + 1:bits / BITS_PER_WORD)

typedef struct cbitmap_struct {
  unsigned long *array;
  unsigned int nbits;
} cbitmap_t;

//--------------//
// init_cbitmap //
//--------------//

// init_cbitmap creates a new cbitmap structure which is held in the
// "buffer" memory area. Buffer's size must be, at least, ARRAY_SIZE;
static inline void init_cbitmap (cbitmap_t *b, unsigned long *buffer,
                                unsigned int nbits) {
  b -> nbits = nbits;
  b -> array = buffer;
  memset ((unsigned char *) buffer, 0, sizeof (unsigned long) *
          ARRAY_SIZE(nbits));
}

//---------------//
// clear_cbitmap //
//---------------//

static inline void clear_cbitmap (cbitmap_t *b) {
  memset ((unsigned char *) b -> array, 0, sizeof (unsigned long) *
          ARRAY_SIZE(b -> nbits));
}

//-------------//
// set_cbitmap //
//-------------//

static inline void set_cbitmap (cbitmap_t *b) {
  memset ((unsigned char *) b-> array,  ~0, sizeof (unsigned long) *
          ARRAY_SIZE(b -> nbits));
}

//----------------//
// setbit_cbitmap //
//----------------//

static inline int setbit_cbitmap (cbitmap_t *b, unsigned int bitp) {
  unsigned int a_entry = bitp / BITS_PER_WORD, bit = bitp % BITS_PER_WORD;
  if (bitp >= b -> nbits) return -1;

  b -> array [a_entry] |= (1 << bit);

  return 0;
}

//------------------//
// clearbit_cbitmap //
//------------------//

static inline int clearbit_cbitmap (cbitmap_t *b, unsigned int bitp) {
  unsigned int a_entry = bitp / BITS_PER_WORD, bit = bitp % BITS_PER_WORD;
  if (bitp >= b -> nbits) return -1;

  b -> array [a_entry] &= ~(1 << bit);
  return 0;
}

//------------------//
// isbitset_cbitmap //
//------------------//

static inline int isbitset_cbitmap (cbitmap_t *b, unsigned int bitp) {
  unsigned int a_entry = bitp / BITS_PER_WORD, bit = bitp % BITS_PER_WORD;
  if (bitp >= b -> nbits) return 0;

  return (b -> array [a_entry] & (1 << bit));
}

//-----------------//
// findfbs_cbitmap //
//-----------------//

// findfbs: finds the first bit set
static inline int findfbs_cbitmap (cbitmap_t *b) {
  unsigned int a_entry = 0, bit = -1, array_size = ARRAY_SIZE (b -> nbits);

  while (bit == -1 && a_entry < array_size) {
    if (!b -> array [a_entry])
      a_entry ++;
    else
      bit = _ffs (b -> array [a_entry]) + a_entry * BITS_PER_WORD;
  }

  return bit;
}

//-----------------//
// findnbs_cbitmap //
//-----------------//

// findnbs: finds the next bit set
static inline int findnbs_cbitmap (cbitmap_t *b, int bitp) {
  unsigned int a_entry, bit;
  unsigned long tmp;

  bitp ++;
  a_entry = bitp/ BITS_PER_WORD;
  bit = bitp % BITS_PER_WORD;


  if (bitp >= b -> nbits) return -1;

  if (bit) {
    tmp = b -> array [a_entry];
    tmp >>= bit;
    if (tmp)
      goto finish;
  }
  a_entry ++;
  while (!b -> array[a_entry] &&
         a_entry < ARRAY_SIZE (b -> nbits))
    a_entry ++;


  if (a_entry >= ARRAY_SIZE (b -> nbits)) return -1;

  bit = 0;
  tmp = b -> array [a_entry];

 finish:
  bit += a_entry * BITS_PER_WORD;
  while (!(tmp & 0xff)) {
    bit += 8;
    tmp >>= 8;
  }

  while (!(tmp & 1)) {
    bit++;
    tmp >>= 1;
  }

  return bit;
}

#endif
