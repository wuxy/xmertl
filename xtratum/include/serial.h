/*
 * $FILE: rtfifo.h 
 *
 * Offer the basic fifo memory and operations
 *
 * $VERSION: 1.0
 *
 * Author: Shuwei Bai<baishuwei@dslab.lzu.edu.cn> 
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

#ifndef __XM_SERIAL_H__
#define __XM_SERIAL_H__

#ifndef PAGE_SIZE 
#define PAGE_SIZE 4096
#endif

#define SERIAL_BASE_ADDR 0x3F8
#define SERIAL_PORT_SIZE 8

typedef struct 
serial_fifo_struct 
{
	unsigned int flag;  /* FIFO state flag */
        long top;      	/* point to the first free address */
        long bom;      	/* point to the first data address */
        long size;     	/* fifo size */
	long baud;	/* baudrate for the device */
	long prio; 	/* priority */	
	char *addr;	/* fifo data memory address, 
			 * global address, don't change it */
} serial_fifo_t;

typedef struct
dom_fifo_struct
{
	serial_fifo_t *serf;
	char *addr; 	/* specify the fifo data memory, local address */
} dom_fifo_t;

#ifdef __XM_KERNEL__

typedef struct
xm_serial_struct
{
	unsigned int flag;
	domain_t *dom;
	dom_fifo_t in;
	dom_fifo_t out;
} xm_serial_t;

#else

typedef struct
rt_serial_struct
{
	unsigned int flag;
	dom_fifo_t in;
	dom_fifo_t out;
} rt_serial_t;

#endif

/* RT_* macros are used in the domain space */

#define RT_SERIAL_unUsed 	0 /* */
#define RT_SERIAL_dev_INIT 	1 /* */
#define RT_SERIAL_dom_INIT 	2 /* */
#define RT_SERIAL_dom_USED 	3 /* */
#define RT_SERIAL_dom_READ 	4 /* device read permission */
#define RT_SERIAL_dom_WRITE 	5 /* device write permission */

#define RT_SERIAL_dom_DEST_MASK 0xFF000000

/* XM_* macros are used in serial module and domain low level*/
#define XM_SERIAL_FIFO_FLUSH 0x2

#define XM_SERIAL_DEV_USED 1 /* The serial device has been \
				initialized(Set by serial device driver) */

#define XM_HSERIAL_DEV_MASK 0xF0000000 /* 16 hardware serial device */

#define ADDR_UNVALID(addr) (addr == NULL)

#define FIFO_ADDR(ff) ((ff)->addr)
#define FIFO_BOM(ff)  ((ff)->bom)
#define FIFO_TOP(ff)  ((ff)->top)
#define FIFO_SIZE(ff) ((ff)->size)
#define FIFO_USED_SIZE(ff) ((FIFO_TOP(ff) + FIFO_SIZE(ff) - FIFO_BOM(ff)) % FIFO_SIZE(ff)) 
#define FIFO_FREE_SIZE(ff) ((FIFO_BOM(ff) + FIFO_SIZE(ff) - FIFO_TOP(ff) - 1) % FIFO_SIZE(ff))

// The valid memory size is (FIFO_SIZE - 1). Why? :

extern int rt_console_get(int minor, unsigned long *in, unsigned long *out);
extern int rt_console_free(int minor);

#endif
