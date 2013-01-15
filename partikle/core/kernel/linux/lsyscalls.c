/*
 * $FILE: lsyscalls.c
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

#include <arch/linux_syscalls.h>
#include <stddef.h>
#include "lsyscalls.h"

void linux_exit(int value) {
  __asm__ volatile ("int $0x80\n\t" \
		    : : "a" (1),"b" ((long)(value)));
}

_syscall6(void *, mmap2, void *, start, unsigned int, length, int, prot, int, flags, int, fd, int, offset);

_syscall3(int, open, const char *, pathname, int, flags, int, mode);

_syscall1(int, close, int, fd);

_syscall3(ssize_t, write, int, fd, const void *, buf, size_t, count);

_syscall3(ssize_t, read, int, fd, void*, buf, size_t, count);

_syscall3(int, setitimer, int, which, const struct linux_itimerval *, 
	  value, struct linux_itimerval *, ovalue);

_syscall3(int, sigprocmask, int, how, const linux_sigset_t *, set, 
	  linux_sigset_t *, oldset);

_syscall2(int, getitimer, int, which, struct linux_itimerval *, value);

_syscall3(int, sigaction, int, signum, const struct linux_sigaction *, act, 
	  struct linux_sigaction *, oldact);

_syscall1(int, time, int *, t);

_syscall2 (int, munmap, void *, start, size_t, length);

_syscall1 (int, iopl, int, level);

_syscall0 (int, pause);



int linux_sigfillset (linux_sigset_t *set) {
  set->__val[0]=(unsigned long)-1;
  if (_LINUX_SIGSET_NWORDS>1) set->__val[1]=(unsigned long)-1;
  if (_LINUX_SIGSET_NWORDS>2) {
    set->__val[2]=(unsigned long)-1;
    set->__val[3]=(unsigned long)-1;
  }
  return 0;
}
