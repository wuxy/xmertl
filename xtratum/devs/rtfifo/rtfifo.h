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

#include <config.h>

#ifndef __RT_FIFO_H__
#define __RT_FIFO_H__

#define FIFO_RD 0x1  		 /* read flag */
#define FIFO_WR (0x2 | FIFO_RD)  /* write flag */
#define FIFO_UU 0x4  		 /* un-usable  */
#define FIFO_US 0x8  		 /* usable */
#define FIFO_RL 0x10 		 /* rtlinux->linux */
#define FIFO_LR 0x20 		 /* linux->rtlinux */

#define FLUSH_FIFO   0x01

struct fifo_struct {
        unsigned long ff_flags;	/* fifo flags for fifo operation */
        long ff_top;      	/* point to the first free address */
        long ff_bom;      	/* point to the first data address */
        long ff_size;     	/* fifo size */
        char *ff_addr;		/* fifo data memory address  */
};

typedef struct fifo_struct *fifo_t;

#define INDEX_UNVALID(i) (i < 0 || i >= XM_FIFO_COUNT)
#define ADDR_UNVALID(addr) (addr == NULL)

#define FIFO_FLAG(ff) ((ff)->ff_flags) 
#define FIFO_ADDR(ff) ((ff)->ff_addr)
#define FIFO_BOM(ff)  ((ff)->ff_bom)
#define FIFO_TOP(ff)  ((ff)->ff_top)
#define FIFO_SIZE(ff) ((ff)->ff_size)
#define FIFO_USED_SIZE(ff) ((FIFO_TOP(ff) + FIFO_SIZE(ff) - FIFO_BOM(ff)) % FIFO_SIZE(ff)) 
#define FIFO_FREE_SIZE(ff) ((FIFO_BOM(ff) + FIFO_SIZE(ff) - FIFO_TOP(ff) - 1) % FIFO_SIZE(ff))

// The valid memory size is (FIFO_SIZE - 1). Why? :)

#define FIFO_USABLE(ff) ((ff)->ff_flags & FIFO_US)
#define FIFO_READING(ff) ((ff)->ff_flags & FIFO_RD)
#define FIFO_WRITING(ff) ((ff)->ff_flags & FIFO_WR)
#define SET_READ_FLAGS(ff) ((ff)->ff_flags |= FIFO_RD)
#define SET_WRITE_FLAGS(ff) ((ff)->ff_flags |= FIFO_WR)
#define UNSET_READ_FLAGS(ff) ((ff)->ff_flags &= ~FIFO_RD)
#define UNSET_WRITE_FLAGS(ff) ((ff)->ff_flags &= ~FIFO_WR)


#define FIFO_VALUE_INIT(ff, addr, size) \
        do { \
                (ff)->ff_flags |= FIFO_US; \
                (ff)->ff_bom = 0; \
                (ff)->ff_top = 0; \
                (ff)->ff_size = size; \
                (ff)->ff_addr = addr; \
        } while(0)

#define CAS(adr, ov, nv) ({ \
	__typeof__(ov) ret; \
	__asm__ __volatile__( \
	"cmpxchg %3,%1" \
	:"=a"(ret),"+m"(*(volatile unsigned long *)(adr)) \
	:"a"(ov),"r"(nv)); \
	ret; \
	})

extern int rt_fifo_user_read(int index, char *dest, int size);
extern int rt_fifo_user_write(int index, const char *src, int size);
extern int rt_fifo_user_ioctl(int index, int cmd, void *para);


#endif


