/*
 * $FILE: mm.c
 *
 * XM's Memory manager
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

#include <mm.h>
#include <linux_inc.h>

//---------------//
// mm_alloc_page //
//---------------//

unsigned long mm_alloc_page (void) {
  unsigned long pg = __get_free_page (GFP_ATOMIC);
  if (pg)
    memset ((unsigned char *)pg, 0, PAGE_SIZE);
  return pg;
}

//-----------------//
// mm_release_page //
//-----------------//

void mm_release_page (unsigned long pg) {
  free_page (pg);
}
