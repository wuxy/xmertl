/*
 * $FILE: log.h
 *
 * A heap implementation
 *
 * $VERSION: 1.0
 *
 * Author: Bai Shuwei<baishuwei@dslab.lzu.edu.cn>
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
 **/

#ifndef __XM_LOG_H__
#define __XM_LOG_H__

#define XMLOG(expr)	\
do {	\
	printk("<XtratuM> %s", expr);	\
} while(0)

#define XMBUG()	\
do {	\
	printk("<XtratuM> BUG: %s(%d) %s\n", __FILE__, __LINE__, __func__);	\
} while(0)

#endif
