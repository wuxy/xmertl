/*
 * $FILE:  
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

#include <linux/fs.h>
#include <linux/cdev.h>

#ifndef __LINUX_SHM_H__

#define __LINUX_SHM_H__

#include <xmdev.h>

#ifdef XM_SHM_COUNT
# define LINUX_SHM_COUNT XM_SHM_COUNT
#else
# define LINUX_SHM_COUNT 16
#endif

#define LSHM_MAJOR 156
#define LSHM_MINOR 0


struct lshm_t
{
	unsigned long shm_addr;
	unsigned long shm_size;
	int          shm_users;
	int        shm_xmminor;
};

struct linux_shm_dev_struct
{
	struct lshm_t lshm[LINUX_SHM_COUNT];
	struct cdev cdev;
};

typedef struct linux_shm_dev_struct lshm_dev_t;
#endif
