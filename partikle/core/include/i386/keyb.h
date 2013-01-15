/*
 * $FILE: keyb.h
 *
 * Keyboard
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

#ifndef _KEYB_H_
#define _KEYB_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

// Control registers
#define KBD_STATUS_REG 0x64
#define KBD_CNTL_REG 0x64
#define KBD_DATA_REG 0x60

// Commands
#define KBD_READ_MODE 0x20
#define KBD_WRITE_MODE 0x60
#define KBD_SELF_TEST 0xAA
#define KBD_SELF_TEST2 0xAB
#define KBD_CNTL_ENABLE 0xAE
#define KBD_ENABLE 0xF4
#define KBD_DISABLE 0xF5
#define KBD_RESET 0xFF

// Keyboard replies
#define KBD_ACK 0xFA
#define KBD_POR 0xAA

// Status registes bits
#define KBD_OBF 0x01
#define KBD_IBF 0x02
#define KBD_GTO 0x40
#define KBD_PERR 0x80

// Controller mode registers
#define KBD_EKI 0x01
#define KBD_SYS 0x04
#define KBD_DMS 0x20
#define KBD_KCC 0x40

#endif
