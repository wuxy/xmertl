/*
 * $FILE: text_vga.h
 *
 * VGA text mode
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

#ifndef _TEXT_VGA_H_
#define _TEXT_VGA_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <arch/io.h>
#include <arch/paging.h>

// exported functions
int clr_terminal (void);
int print_screen (const char *str, unsigned long length);

#define TEXT_VGA_ADDRESS ((unsigned char *) (0xb8000 + OFFSET))

#define R_MISC_OUTPUT 0x3cc
#define W_MISC_OUTPUT 0x3c2

#define CRTC_COM_REG 0x3d4
#define CRTC_DATA_REG 0x3d5
#define CURSOR_LOC_H 0x0e
#define CURSOR_LOC_L 0x0f
#define START_ADDR_H 0x0c
#define START_ADDR_L 0x0d

#define SEQ_COM_REG 0x3c4
#define SEQ_DATA_REG 0x3c5
#define CHAR_MAP_SELEC 0x3

#define vga_set_cursor_pos(cursor_pos) ({ \
  outb_p (CURSOR_LOC_H, CRTC_COM_REG); \
  outb_p ((unsigned char)((cursor_pos >> 8)) & 0xff, CRTC_DATA_REG); \
  outb_p (CURSOR_LOC_L, CRTC_COM_REG); \
  outb_p ((unsigned char)(cursor_pos & 0xff), CRTC_DATA_REG); \
})

#define vga_get_cursor_pos() ({ \
  unsigned char tmp_H = 0, tmp_L = 0; \
  unsigned short tmp; \
  outb_p (CURSOR_LOC_H, CRTC_COM_REG); \
  tmp_H = inb_p (CRTC_DATA_REG); \
  outb_p (CURSOR_LOC_L, CRTC_COM_REG); \
  tmp_L = inb_p (CRTC_DATA_REG); \
  tmp = (unsigned short) ((tmp_H << 8) | tmp_L); \
  tmp; \
})

#define vga_set_start_addr(start_addr) ({ \
  outb_p (START_ADDR_H, CRTC_COM_REG); \
  outb_p ((unsigned char)((start_addr >> 8)) & 0xff, CRTC_DATA_REG); \
  outb_p (START_ADDR_L, CRTC_COM_REG); \
  outb_p ((unsigned char)(start_addr & 0xff), CRTC_DATA_REG); \
})

#define vga_get_start_addr() ({ \
  unsigned char tmp_H = 0, tmp_L = 0; \
  unsigned short tmp; \
  outb_p (START_ADDR_H, CRTC_COM_REG); \
  tmp_H = inb_p (CRTC_DATA_REG); \
  outb_p (START_ADDR_L, CRTC_COM_REG); \
  tmp_L = inb_p (CRTC_DATA_REG); \
  tmp = (unsigned short) ((tmp_H << 8) | tmp_L); \
  tmp; \
})

#define vga_set_char_map_select() ({ \
  unsigned char tmp = 0; \
  outb_p (CHAR_MAP_SELEC, SEQ_COM_REG); \
  tmp = (unsigned char) inb_p (SEQ_DATA_REG); \
  tmp; \
})

#endif // _TEXT_VGA_H_
