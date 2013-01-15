/*
 * $FILE: config.h
 *
 * XM's configuration file
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

#ifndef _XM_CONF_H_
#define _XM_CONF_H_

// MAX_EVENTS defines the  maximum number of events that  a domain can
// receive, it includes hardware interrupts and traps
#define MAX_EVENTS 32

#define DEFAULT_SSTACK_SIZE 12288
/*#define DEFAULT_USTACK_ADDR 0x2000000 // 32 MByte
  #define DEFAULT_USTACK_SIZE 12288*/

#define DEFAULT_EVENT_ADDR 0x2000000 // 32 MByte

#define DEFAULT_HEAP_SIZE 3145728 // 3 MBytes

#define XM_FIFO_COUNT   16
#define DEFAULT_FIFO_ADDR 0x2C00000 // == 44M

#define XM_SHM_COUNT    16
#define DEFAULT_SHM_ADDR  0x3000000 // == 48M

#define XM_SERIAL_COUNT 16
#define DEFAULT_SERIAL_ADDR 0x3400000 // == 52M

#define XM_FIFO_DEV_SUPPORT	1	
#define XM_SHM_DEV_SUPPORT	0
#define XM_SERIAL_DEV_SUPPORT   1		

#if (XM_FIFO_DEV_SUPPORT | XM_SHM_DEV_SUPPORT | XM_SERIAL_DEV_SUPPORT) 
#define XM_DEV_SUPPORT
#endif 

#define CONFIG_FP_SUPPORT

#endif
