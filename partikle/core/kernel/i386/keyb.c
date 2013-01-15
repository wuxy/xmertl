/*
 * $FILE: keyb.c
 *
 * i386 keyboard driver
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
#include <keymap.h>
#include <processor.h>
#include <stdio.h>
#include <arch/io.h>
#include <arch/keyb.h>
#include <arch/atomic.h>
#include <irqs.h>

#define KEYB_IRQ 1

#define KEYB_BUFSIZE 256

static char keyb_buffer [KEYB_BUFSIZE];
static int buftail = 0, bufhead = 0, bufsize = 0;
static int echo_enable = 1;

static inline void insert_keyb_buffer (char c) {
  int __flags;
  hw_save_flags_and_cli (__flags);
  if (bufsize >= KEYB_BUFSIZE)
    buftail = bufhead = bufsize = 0;
  bufsize ++;
  keyb_buffer [buftail] = c;
  buftail = (buftail + 1) % KEYB_BUFSIZE; 
  hw_restore_flags (__flags);
}

static inline char extract_keyb_buffer (void) {
  char __c;
  int __flags;
  hw_save_flags_and_cli (__flags);
  if (bufsize <= 0) {
    hw_restore_flags (__flags);
    return -1;
  }
  bufsize --;
  __c = keyb_buffer [bufhead];
  bufhead = (bufhead + 1) % KEYB_BUFSIZE;
  hw_restore_flags (__flags);
  return __c;
}

static int keyb_handler (context_t *context) {
  static unsigned int shift_state = 0;
  static unsigned int escaped = 0;
  unsigned scan_code, new_char;

  // Nothing to read
  if (!(inb(KBD_STATUS_REG) & KBD_OBF))
    return -1;
  
  scan_code = inb (KBD_DATA_REG);

  if (escaped) scan_code += 256;
  
  switch (scan_code) {
  case 0x2a: 
  case 0x36:
    shift_state = 1; break;
  case 0xaa: 
  case 0xb6:
    shift_state = 0; break;
  case 0xe0: escaped = 1; break;
  default:
    if (scan_code & 0x80) {
      // ignore the break code
    } else {
      new_char = keymap[scan_code][shift_state];
      insert_keyb_buffer (new_char);
      //printf ("%c - %x ", new_char, scan_code);
    }
    break;
  }
  
  return 0;
}

//-----------//
// keyb_read //
//-----------//

int keyb_read (char *str, unsigned long length) {
  int n;
  volatile char ch;
  // Just one task can be read from the keyboard at the same time
  /*
   * TODO: Use a mutex to avoid race conditions
   */

  // mutext_lock
  for (n = 0; n < length; n++) {
    /*
     * TODO: Busy waiting, it is not very efficient, reimplement it in a
     * more efficient way
     */
    do {
      ch = extract_keyb_buffer ();
    } while (ch == -1);
    str [n] = ch;
    if (echo_enable) printf ("%c", ch);
  }

  // mutext_unlock
  return n;
}

//----------//
// keyblink //
//----------//

int keyblink (void) {
  unsigned long flags;
  
  hw_save_flags_and_cli (flags);
  install_irq_handler_sys (KEYB_IRQ, keyb_handler);
  hw_restore_flags (flags);

  hw_enable_irq (KEYB_IRQ);
  return 0;
}

