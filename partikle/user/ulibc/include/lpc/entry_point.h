/*
 * $FILE: entry_point.h
 *
 * Real entry point definition
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
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

// This header file defines the way to call PaRTiKle's functions

#ifndef _U_ENTRY_POINT_H_
#define _U_ENTRY_POINT_H_

#include <stdlib.h>

extern int main (int, char **);
extern void finish_lang_supp (void);

// get rid of ctors/dtors of OO langs
// (see lang_supp.c:/init_lang_supp for all the details)
unsigned long __CTOR_START__ [1]={-1};
unsigned long __CTOR_END__ [1]={-1};
unsigned long __DTOR_LIST__[2] = {0};
	
#define ENTRY_POINT() \
  int user_entry_point (void) { \
    int ret; \
    init_ulibc (); \
    ret=main (0,0); \
    finish_lang_supp (); \
    exit (ret); \
    return 0; \
  }

#endif
