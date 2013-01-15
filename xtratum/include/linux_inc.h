/*
 * $FILE: linux_inc.h
 *
 * Linux's headers included by XM
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

#ifndef _XM_LINUX_INC_H_
#define _XM_LINUX_INC_H_

#include <linux/config.h>
#include <asm/xtratum.h>

#include <linux/module.h>

#include <linux/smp.h>
#include <linux/gfp.h>
#include <linux/irq.h>
#include <linux/vmalloc.h>
#include <linux/console.h>
#include <linux/module.h>
#include <linux/timex.h>
#include <linux/limits.h>
//#include <linux/wrapper.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>

#include <asm/desc.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define SYMBOL_NAME_STR(X) #X

#define __STR(x) #x
#define STR(x) __STR(x)

#endif // _LINUX_INC_H_
