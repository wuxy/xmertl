/*
 * $FILE: vga.c
 *
 * VGA Driver
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

#include <config.h>
#include <string.h>
#include <koutput.h>
#include "vga.h"

#define COLUMNS 80
#define LINES 25
#define ATTRIBUTE 7

#define BUFFER_SIZE COLUMNS * LINES * 2

static unsigned char *buffer;
static int xpos, ypos;

//--------------//
// clr_terminal //
//--------------//

int clr_terminal (void) {
  int pos;

  buffer = (unsigned char *) TEXT_VGA_ADDRESS;
  xpos = ypos = 0;
  for (pos = 0; pos < COLUMNS * LINES; pos ++)
    ((unsigned short *)buffer) [pos] = (ATTRIBUTE << 8);

  vga_set_start_addr (0);
  vga_set_cursor_pos (0);
  return 0;
}

//------------------//
// putchar_terminal //
//------------------//

static void putchar_terminal (int c) {
  int pos;
  
  if (c == '\t') {
    xpos += 3;
    if (xpos >= COLUMNS)
      goto newline;
    vga_set_cursor_pos ((xpos + ypos * COLUMNS));
    return;
  }

  if (c == '\n' || c == '\r') {
  newline:
    xpos = 0;
    ypos++;
    if (ypos == LINES) {
      memcpy ((unsigned char *) buffer, 
	      (unsigned char *) &buffer [COLUMNS * 2],
	      (LINES - 1) * COLUMNS * 2);
      
      for (pos = 0; pos < COLUMNS; pos ++)
	((unsigned short *) buffer) [pos + (LINES - 1) * COLUMNS] = 
	  (ATTRIBUTE << 8);
      ypos--;
    }
    
    vga_set_cursor_pos ((xpos + ypos * COLUMNS));
    return;
  }
  
  buffer [(xpos + ypos * COLUMNS) * 2] = c & 0xFF;
  buffer [(xpos + ypos * COLUMNS) * 2 + 1] = ATTRIBUTE;

  xpos++;
  if (xpos >= COLUMNS)
    goto newline;
  
  vga_set_cursor_pos ((xpos + ypos * COLUMNS));
}


//--------------//
// print_screen //
//--------------//

int print_screen (const char *str, unsigned long length) {
  int n;
  for (n = 0; n < length; n++)
    putchar_terminal (str [n]);
  return length;
}

