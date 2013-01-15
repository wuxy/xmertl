/*
 * $FILE: syscalls.h
 *
 * System calls, Linux version
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

// This header file defines the way to call PaRTiKle's functions

#ifndef _ULIBC_LINUX_I386_SYSCALLS_H_
#define _ULIBC_LINUX_I386_SYSCALLS_H_

#define __syscall_return(type, res) \
  return (type) (res);


#define __syscall0(name_nr, __res) \
  __asm__ __volatile__ ("call syscall_gate\n\t" \
                        : "=a" (__res) \
                        : "0" (name_nr))

#define __syscall1(arg1, name_nr, __res) \
  __asm__ __volatile__ ("call syscall_gate\n\t" \
                        : "=a" (__res) \
                        : "0" (name_nr),"b" ((long)(arg1)))

#define __syscall2(arg1, arg2, name_nr, __res) \
  __asm__ __volatile__ ("call syscall_gate\n\t" \
			: "=a" (__res) \
			: "0" (name_nr),"b" ((long)(arg1)), \
			"c" ((long)(arg2)))

#define __syscall3(arg1, arg2, arg3, name_nr, __res) \
  __asm__ __volatile__ ("call syscall_gate\n\t" \
			: "=a" (__res) \
			: "0" (name_nr),"b" ((long)(arg1)), \
			"c" ((long)(arg2)), \
			"d" ((long)(arg3)))

#define __syscall4(arg1, arg2, arg3, arg4, name_nr, __res) \
  __asm__ __volatile__ ("call syscall_gate\n\t" \
			: "=a" (__res) \
			: "0" (name_nr),"b" ((long)(arg1)), \
			"c" ((long)(arg2)), \
			"d" ((long)(arg3)),"S" ((long)(arg4)))

#define __syscall5(arg1, arg2, arg3, arg4, arg5, name_nr, __res) \
  __asm__ __volatile__ ("call syscall_gate\n\t" \
			: "=a" (__res) \
			: "0" (name_nr),"b" ((long)(arg1)), \
			"c" ((long)(arg2)), \
			"d" ((long)(arg3)),"S" ((long)(arg4)), \
			"D" ((long)(arg5)))


/******************/

#define _syscall0(type,name) \
type name(void) { \
  long __res; \
  __syscall0(name##_nr, __res); \
  __syscall_return(type,__res); \
}

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) { \
  long __res; \
  __syscall1(arg1, name##_nr, __res); \
  __syscall_return(type,__res); \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 arg1,type2 arg2) { \
  long __res; \
  __syscall2(arg1, arg2, name##_nr, __res); \
  __syscall_return(type,__res); \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) { \
  long __res; \
  __syscall3(arg1, arg2, arg3, name##_nr, __res); \
  __syscall_return(type,__res); \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 arg1, type2 arg2, type3 arg3, type4 arg4) { \
  long __res; \
  __syscall4(arg1, arg2, arg3, arg4, name##_nr, __res); \
  __syscall_return(type,__res); \
}

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5) { \
  long __res; \
  __syscall5(arg1, arg2, arg3, arg4, arg5, name##_nr, __res); \
  __syscall_return(type,__res); \
}

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
				          type5,arg5,type6,arg6) \
type name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5,type6 arg6) \
{ \
	long __res; \
		__asm__ volatile ("push %%ebp ; movl %%eax,%%ebp ; movl %1,%%eax ; call syscall_gate; pop %%ebp" \
		: "=a" (__res) \
		: "i" (name##_nr),"b" ((long)(arg1)),"c" ((long)(arg2)), \
		"d" ((long)(arg3)),"S" ((long)(arg4)),"D" ((long)(arg5)), \
		"0" ((long)(arg6))); \
	return (type) (__res); \
}

#endif
