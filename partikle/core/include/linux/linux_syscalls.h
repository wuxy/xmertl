/*
 * $FILE: linux_syscalls.h
 *
 * Definition of the HAL interface
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

#ifndef _LINUX_SYSCALLS_H_
#define _LINUX_SYSCALLS_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <stddef.h>
//#include <time.h>

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 0100
#define O_TRUNC 01000
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define MAP_ANONYMOUS 0x20
#define MAP_PRIVATE 0x02

#define LINUX_SA_RESTART 0x10000000u
#define LINUX_SA_SIGINFO 0x00000004u
#define LINUX_SIGALARM 14
#define LINUX_SIGSEGV 11
#define LINUX_SIGBUS 7

#define LINUX_SIG_UNBLOCK 1

#define _LINUX_SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))

typedef struct {
    unsigned long int __val[_LINUX_SIGSET_NWORDS];
} linux_sigset_t;

#define _LINUX_SI_MAX_SIZE     128
#define _LINUX_SI_PAD_SIZE     ((_LINUX_SI_MAX_SIZE/sizeof(int)) - 3)

typedef struct linux_siginfo {
  int si_signo;
  int si_errno;
  int si_code;
  union {
    int _pad[_LINUX_SI_PAD_SIZE];
    /* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
    struct {
      void *_addr; /* faulting insn/memory ref. */
    } _sigfault;
  } _sifields;
} linux_siginfo_t;

struct linux_sigaction {
  union {
    /* Used if SA_SIGINFO is not set.  */
    void (*linux_sa_handler) (int);
    /* Used if SA_SIGINFO is set.  */
    void (*linux_sa_sigaction) (int, linux_siginfo_t *, void *);
  } __sigaction_handler;
  linux_sigset_t sa_mask;
  int sa_flags;
  void (*linux_sa_restorer)(void);
};

struct linux_timeval {
  long tv_sec;                /* seconds */
  long tv_usec;               /* microseconds */
};

struct linux_itimerval {
  struct linux_timeval it_interval; /* next value */
  struct linux_timeval it_value;    /* current value */
};

extern void linux_exit(int value);
extern void *linux_mmap2(void *start, unsigned int length, int prot, 
		 	 int flags, int fd, int offset);
extern int linux_munmap(void *start, size_t length);
extern int linux_open(const char *pathname, int flags, int mode);
extern ssize_t linux_read(int fd, void *buf, size_t count);
extern ssize_t linux_write(int fd, const void *buf, size_t count);
extern int linux_close(int fd);
extern int linux_getitimer(int which, struct linux_itimerval *value);
extern int linux_setitimer(int which, const struct linux_itimerval *value, 
			   struct linux_itimerval *ovalue);
extern int linux_sigaction(int signum, const struct linux_sigaction *act,
                           struct linux_sigaction *oldact);
extern int linux_time (int *);
extern int linux_sigfillset (linux_sigset_t *set);
extern int linux_iopl (int level);
extern int linux_sigprocmask(int how, const linux_sigset_t *set,
			     linux_sigset_t *oldset);
extern int linux_pause(void);
#endif

