/*
 * $FILE: keymap.h
 *
 * Keyboard maps
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

#ifndef _KEYMAP_H_
#define _KEYMAP_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <config.h>

#ifdef ES_KEYBMAP

/*
 * TODO: Define the spanish keyboard map
 */

#error The spanish keyboard map has not been defined yet

#else // default option: US_KEYBMAP

static char keymap [128][2] = {
  {0},			/* 0 */
  {27,	27},		/* 1 - ESC */
  {'1',	'!'},		/* 2 */
  {'2',	'@'},
  {'3',	'#'},
  {'4',	'$'},
  {'5',	'%'},
  {'6',	'^'},
  {'7',	'&'},
  {'8',	'*'},
  {'9',	'('},
  {'0',	')'},
  {'-',	'_'},
  {'=',	'+'},
  {8,	8},		/* 14 - Backspace */
  {'\t', '\t'},		/* 15 */
  {'q',	'Q'},
  {'w',	'W'},
  {'e',	'E'},
  {'r',	'R'},
  {'t',	'T'},
  {'y',	'Y'},
  {'u',	'U'},
  {'i',	'I'},
  {'o',	'O'},
  {'p',	'P'},
  {'[',	'{'},
  {']',	'}'},		/* 27 */
  {'\r', '\r'},		/* 28 - Enter */
  {0,	0},		/* 29 - Ctrl */
  {'a',	'A'},		/* 30 */
  {'s',	'S'},
  {'d',	'D'},
  {'f',	'F'},
  {'g',	'G'},
  {'h',	'H'},
  {'j',	'J'},
  {'k',	'K'},
  {'l',	'L'},
  {';',	':'},
  {'\'', '"'},		/* 40 */
  {'`',	'~'},		/* 41 */
  {0, 0},	/* 42 - Left Shift */
  {'\\', '|'},		/* 43 */
  {'z',	'Z'},		/* 44 */
  {'x',	'X'},
  {'c',	'C'},
  {'v',	'V'},
  {'b',	'B'},
  {'n',	'N'},
  {'m',	'M'},
  {',',	'<'},
  {'.',	'>'},
  {'/',	'?'},		/* 53 */
  {0, 0},	/* 54 - Right Shift */
  {0, 0},		/* 55 - Print Screen */
  {0, 0},		/* 56 - Alt */
  {' ',	' '},		/* 57 - Space bar */
  {0, 0},		/* 58 - Caps Lock */
  {0, 0},		/* 59 - F1 */
  {0, 0},		/* 60 - F2 */
  {0, 0},		/* 61 - F3 */
  {0, 0},		/* 62 - F4 */
  {0, 0},		/* 63 - F5 */
  {0, 0},		/* 64 - F6 */
  {0, 0},		/* 65 - F7 */
  {0, 0},		/* 66 - F8 */
  {0, 0},		/* 67 - F9 */
  {0, 0},		/* 68 - F10 */
  {0, 0},		/* 69 - Num Lock */
  {0, 0},		/* 70 - Scroll Lock */
  {'7',	'7'},		/* 71 - Numeric keypad 7 */
  {'8',	'8'},		/* 72 - Numeric keypad 8 */
  {'9',	'9'},		/* 73 - Numeric keypad 9 */
  {'-',	'-'},		/* 74 - Numeric keypad '-' */
  {'4',	'4'},		/* 75 - Numeric keypad 4 */
  {'5',	'5'},		/* 76 - Numeric keypad 5 */
  {'6',	'6'},		/* 77 - Numeric keypad 6 */
  {'+',	'+'},		/* 78 - Numeric keypad '+' */
  {'1',	'1'},		/* 79 - Numeric keypad 1 */
  {'2',	'2'},		/* 80 - Numeric keypad 2 */
  {'3',	'3'},		/* 81 - Numeric keypad 3 */
  {'0',	'0'},		/* 82 - Numeric keypad 0 */
  {'.',	'.'},		/* 83 - Numeric keypad '.' */
};

#endif

#endif
