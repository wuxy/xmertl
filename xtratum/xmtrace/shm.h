/*
 * $FILE: shm.h 
 *
 * $Description:
 *
 * $VERSION: 1.0
 *
 * Author: Bai Shuwei <baishuwei@{dslab.lzu.edu.cn, weslab.com}>
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
 */

#ifndef _XM_SHM_H_
#define _XM_SHM_H_

#include <domain.h>
#include <tlsf.h>
#include <mm.h>

#define SHM_TOTAL_SIZE  (2048*1024)

/*xtratum xm_shm_map command*/

#if 0
# move to the xmdev.h

#define XM_SHM_CREATE   0x01
#define XM_SHM_TRUNCATE 0x02 
#define XM_SHM_MAP      0x04 
#define XM_SHM_UNMAP    0x08
#define XM_SHM_REMOVE   0x10
#define XM_SHM_EXSIT    0x20
#endif

#define SHM_MEM_SIZE  (8*4096)

#define SHM_NAME_SIZE (32+1)

struct xm_shm_struct {
	char name[SHM_NAME_SIZE];     /* SHM Object Name, Max size is 32*/
	domain_t *owner;              /* SHM owner, one shm should have and only one owner */
	unsigned long va_start;       /* SHM start address in the kernel space*/
	unsigned long size;
	unsigned long flags;
	int users;
};

typedef struct xm_shm_struct xm_shm_t;

extern int xm_shm_ioctl(unsigned long index, unsigned long cmd, void *arg);
extern int xm_shm_unlink(int index);
extern int xm_shm_open(const char *name, unsigned long flag, unsigned long mode);

#endif 
