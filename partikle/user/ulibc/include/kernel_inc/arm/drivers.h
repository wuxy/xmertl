/*
 * $FILE: drivers.h
 *
 * Drivers for ARM
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

#ifndef _ARCH_DRIVERS_H_
#define _ARCH_DRIVERS_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <config.h>

#ifdef CONFIG_BOARD_INTEGRATORCP

#ifdef CONFIG_DRV_PL011
#define CONFIG_PL011_CLOCK 14745600
#define CONFIG_PL01x_PORTS {(void *)CFG_SERIAL0, (void *)CFG_SERIAL1}
#define CONFIG_CONS_INDEX 0
#define CONFIG_BAUDRATE 38400
#define CFG_BAUDRATE_TABLE {9600, 19200, 38400, 57600, 115200}
#define CFG_SERIAL0 0x16000000
#define CFG_SERIAL1 0x17000000
#endif

#endif

#endif
