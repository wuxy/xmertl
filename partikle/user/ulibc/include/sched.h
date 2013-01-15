/*
 * $FILE: sched.h
 *
 * Execution scheduler (REALTIME)
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
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

#ifndef _ULIBC_SCHED_H_
#define _ULIBC_SCHED_H_

#include <time.h>
#include <sys/types.h>
#include <kernel_inc/sched.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

extern int sched_get_priority_max(int);
extern int sched_get_priority_min(int);
//extern int sched_getparam(pid_t, struct sched_param *);
//extern int sched_getscheduler(pid_t);
//extern int sched_rr_get_interval(pid_t, struct timespec *);
//extern int sched_setparam(pid_t, const struct sched_param *);
//extern int sched_setscheduler(pid_t, int, const struct sched_param *);
//extern int sched_yield(void);

END_DECLS

#endif
