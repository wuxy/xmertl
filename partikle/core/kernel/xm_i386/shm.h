/*
 * $FILE: shm.h 
 *
 * $Description:
 *   PaRTiKle SHM DEVICE
 *
 * $VERSION: 1.0
 *
 * Author: Bai Shuwei <baishuwi@{dslab.lzu.edu.cn, weslab.com}>
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

#include <arch/xm_dev.h>

#ifndef __PRTK_SHM_H__
#define __PRTK_SHM_H__

#define PRTK_SHM_NAME "shm"

struct pshm_t
{
	unsigned long shm_addr;
	unsigned long shm_size;
	unsigned long shm_flags;
	int          shm_users;
	int        shm_xmminor;
};


struct prtk_shm_dev_struct
{
	struct pshm_t pshm[PRTK_SHM_COUNT];
};

typedef struct prtk_shm_dev_struct pshm_dev_t;



#endif
