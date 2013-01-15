/*
 * $FILE: xm_dev.h
 *    
 * $Description:
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

/* ALL the data in the file should same as in the xmdev.h in XtratuM/include path */

#ifndef __PRTK_XMDEV_H__

#define __PRTK_XMDEV_H__

#ifdef XM_SHM_COUNT
# define PRTK_SHM_COUNT XM_SHM_COUNT
#else
# define PRTK_SHM_COUNT 16
#endif

#ifndef XM_SHM_DEV
# define XM_SHM_DEV 1
#endif

#ifndef _PRTK_SHM_IOCTL_
#define _PRTK_SHM_IOCTL_

/* XM SHM DEVICE IOCTL COMMAND */
#define XM_SHM_CREATE   0x01
#define XM_SHM_TRUNCATE 0x02 
#define XM_SHM_MAP      0x04 
#define XM_SHM_UNMAP    0x08
#define XM_SHM_REMOVE   0x10
#define XM_SHM_EXIST    0x20
#define XM_SHM_VADDR    0x40
#define XM_SHM_SIZE     0x80

#define PRTK_SHM_CREATE   0x01
#define PRTK_SHM_TRUNCATE 0x02 
#define PRTK_SHM_MAP      0x04 
#define PRTK_SHM_UNMAP    0x08
#define PRTK_SHM_REMOVE   0x10
#define PRTK_SHM_EXIST    0x20
#define PRTK_SHM_VADDR    0x40
#define PRTK_SHM_SIZE     0x80

#define DEFAULT_SHM_ADDR 0x3000000
#endif

#endif
