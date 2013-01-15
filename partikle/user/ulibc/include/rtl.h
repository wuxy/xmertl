/*
 * $FILE: rtl.h
 *
 * RTLinux _np (Non Portable) Posix
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
 *
  * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <sched.h>
#include <pthread.h>
#include <signal.h>
#include <kernel_inc/rtl.h>
#include <syscalls.h>

/* 
  * for details: /usr/src/rtlinux-3.2-rc1/doc/posix.txt
  * u grep -rine '_np[ ]*(' /usr/src/rtlinux-3.2-rc1/include
  */

#ifndef _ULIBC_RTLINUX_H_
#define _ULIBC_RTLINUX_H_

// changes to run rtlinux code under partikle
// (useful only for testing)
#define printk	printf
#define rtl_printf printf
#define gethrtime() clock_gethrtime(CLOCK_REALTIME)
typedef hwtime_t  hrtime_t;
//typedef itimerval itimerspec

extern hwtime_t clock_gethrtime (clockid_t clock_id);

// non portable extensions
extern int pthread_wakeup_np (pthread_t thread);
extern int pthread_suspend_np (pthread_t thread);

// CONFIG_FP_SUPPORT enabled from config.h
//extern int pthread_setfp_np (pthread_t thread, int flag);
//extern int pthread_attr_setfp_np (pthread_attr_t *attr, int flag); 
//extern inline int pthread_attr_getfp_np (const pthread_attr_t *attr, int *use_fp);

// multiple processors not supported
//extern inline int pthread_attr_getcpu_np(const pthread_attr_t *attr, int * cpu);
//extern int pthread_attr_setcpu_np(pthread_attr_t *attr, int cpu); //not needed

extern int pthread_getperiod_np (pthread_t thread, duration_t* period);
extern int pthread_setperiod_np (pthread_t thread, struct timespec *value);

extern int pthread_wait_np(void);
extern int pthread_delete_np (pthread_t thread);
extern int pthread_make_periodic_np (pthread_t p, hwtime_t start_time, hwtime_t period);

#endif
