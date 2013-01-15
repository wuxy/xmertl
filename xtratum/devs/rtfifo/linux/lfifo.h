/*
 * $FILE: lfifo.h 
 *
 * Offer the basic real-time fifo memory and operations
 *
 * $VERSION: 1.0
 *
 * Author: Shuwei Bai<baishw06@lzu.cn> 
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


#ifndef _LINUX_FIFO_H_
#define _LINUX_FIFO_H_

#include "../rtfifo.h"


#define FIFO_COUNT XM_FIFO_COUNT

struct linux_fifo_struct {
	int lf_users;
        int lf_flags;
	int lf_index;
        char *lf_addr;
        struct semaphore sem;
};

typedef struct linux_fifo_struct * linux_fifo_t;

#endif

