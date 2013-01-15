/*
 * $FILE: lsyscalls.h
 *
 * Linux's system calls
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

#ifndef _LSYSCALLS_H_
#define _LSYSCALLS_H_

#define __NR_exit 1
#define __NR_write 4
#define __NR_mmap2 192
#define __NR_munmap 91
#define __NR_read 3
#define __NR_write 4
#define __NR_open 5
#define __NR_close 6
#define __NR_time 13
#define __NR_pause 29
#define __NR_sigaction 67
#define __NR_setitimer 104
#define __NR_getitimer 105
#define __NR_iopl 110 
#define __NR_sigprocmask 126

#define _syscall0(type,name) \
type linux_##name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
        : "=a" (__res) \
        : "0" (__NR_##name)); \
  return (type) (__res); \
}

#define _syscall1(type,name,type1,arg1) \
type linux_##name(type1 arg1) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
        : "=a" (__res) \
        : "0" (__NR_##name),"b" ((long)(arg1))); \
  return (type) (__res); \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type linux_##name(type1 arg1,type2 arg2) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
        : "=a" (__res) \
        : "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2))); \
 return (type) (__res); \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type linux_##name(type1 arg1,type2 arg2,type3 arg3) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
        : "=a" (__res) \
        : "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
                  "d" ((long)(arg3))); \
 return (type) (__res); \
}


#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4, \
          type5,arg5,type6,arg6) \
type linux_##name (type1 arg1,type2 arg2,type3 arg3,type4 arg4,type5 arg5,type6 arg6) \
{ \
long __res; \
__asm__ volatile ("push %%ebp ; movl %%eax,%%ebp ; movl %1,%%eax ; int $0x80 ; pop %%ebp" \
        : "=a" (__res) \
        : "i" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
          "d" ((long)(arg3)),"S" ((long)(arg4)),"D" ((long)(arg5)), \
          "0" ((long)(arg6))); \
  return (type) (__res); \
}

#endif
