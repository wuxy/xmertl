/*
 * $FILE: config.h
 *
 * Configuration file
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef _KERNEL_
#include <autoconf.h>
#include <version.h>
#else
#include <kernel_inc/autoconf.h>
#endif

// Stack size per thread, this also includes the size of the main stack
#define STACK_SIZE (16*1024)

// Maximum number of functions that can be registered
// with the atexit function
#define ATEXIT_MAX 32

// Maximum number of handlers that can be registered per
// thread, they will be executed in LIFO order after the end of
// the thread

#define MAX_CLEANUP_HANDLERS 8

// Maximum per threads keys
#define MAX_PTHREAD_KEYS 10

// Maximum number of posix timers
#define MAX_TIMERS 32

#ifdef linux
#define CONFIG_FP_IN_SCANF
#endif

#define US_KEYBMAP

#endif
