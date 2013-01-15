/*
 * $FILE: signal.h
 *
 * Signals
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

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

#include <errno.h>

// PaRTiKle defines up to 32 (31..0) signals

#define NRSIGNALS 32
#define MAX_SIGNAL 31
#define LAST_SYS_SIGNAL 5

/* From 5 to 31 are user-defined signals */
#define FIRST_USER_SIGNAL (LAST_SYS_SIGNAL+1)

#define SIGNAL_NULL 0
#define CANCEL_SIGNAL 1
#define SLEEP_SIGNAL 2
#define WAKEUP_SIGNAL 3
#define RESERVED_SIGNAL1 4
#define RESERVED_SIGNAL2 5

/* User's signal names from 0 to 25 */
enum {
  SIGUSR0 = FIRST_USER_SIGNAL,
  SIGUSR1, SIGUSR2, SIGUSR3, SIGUSR4,
  SIGUSR5, SIGUSR6, SIGUSR7, SIGUSR8,
  SIGUSR9, SIGUSR10, SIGUSR11, SIGUSR12,
  SIGUSR13, SIGUSR14, SIGUSR15, SIGUSR16,
  SIGUSR17, SIGUSR18, SIGUSR19, SIGUSR20,
  SIGUSR21, SIGUSR22, SIGUSR23, SIGUSR24,
  SIGUSR25
};

/* The default triggered signal when a timer reaches 0 */
#define SIGALARM 8

#define LAST_USER_SIGNAL MAX_SIGNAL

/* 
 * SIG_DFL and SIG_IGN are not valid values, since them represents
 * integers and not valid function pointers, sigaction will recognise
 * them, loading suitable values
 */
#define SIG_DFL ((void(*)(int))1)
#define SIG_IGN ((void(*)(int))2)

#define SIG_BLOCK 0
#define SIG_SETMASK 1
#define SIG_UNBLOCK 2

#define SIGEV_SIGNAL 0

union sigval {
  int sival_int;
  void *sival_ptr;
};

typedef struct {
  int si_signo;
  int si_code;
  union sigval si_value;
} siginfo_t;

struct  sigaction {
  union {
    void (*sa_u_handler) (int);
    void (*sa_u_sigaction) (int, siginfo_t *, void *);
  } sa_u;
  sigset_t sa_mask;
  int sa_flags;
};

#define sa_handler sa_u.sa_u_handler
#define sa_sigaction sa_u.sa_u_sigaction

#define SA_SIGINFO 1
#define SA_RESTART 0

struct sigevent {
  int sigev_notify;
  int sigev_signo;
  union sigval sigev_value;
};

#ifdef _KERNEL_

static inline int sigemptyset(sigset_t *set) {
  if (set)
    set -> sig = 0;
  else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  return 0;
}

static inline int sigfillset(sigset_t *set) {
  if (set)
    set -> sig = ~0;
  else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  return 0;
}

static inline int sigaddset(sigset_t *set, int signo) {
  if (set)
    set -> sig |= (1 << signo);
  else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  return 0;
}

static inline int sigdelset(sigset_t *set, int signo) {

  if (set)
    set -> sig &= ~(1 << signo);
  else {
    SET_ERRNO (EINVAL);
    return -1;
  }

  return 0;
}

static inline int sigismember(const sigset_t *set, int signo) {
  unsigned long aux_set;
  if (set) {
    aux_set = set -> sig & (1 << signo);
  } else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  return ((aux_set)?1:0);
}

#include <config.h>
#include <linkage.h>
#include <types.h>

/*
 * All system signals are non maskable
 */

#define non_maskable_signals ((1 << FIRST_USER_SIGNAL) - 1)

struct signal_table_struct {
  union {
    void (*sa_u_handler) (int);
    void (*sa_u_sigaction) (int, siginfo_t *, void *);
  } sa_u;
  sigset_t sa_mask;
};

#define PTHREAD_CANCEL(t) pthread_kill_sys (t, CANCEL_SIGNAL)
#define PTHREAD_WAKEUP(t) pthread_kill_sys (t, WAKEUP_SIGNAL)
#define PTHREAD_SLEEP(t) pthread_kill_sys (t, SLEEP_SIGNAL)

extern struct signal_table_struct signal_table [NRSIGNALS];

extern asmlinkage int kill_sys(int pid, int sig);

extern asmlinkage int sigaction_sys(int sig, const struct sigaction *act,
				    struct sigaction *oact);

extern asmlinkage int sigsuspend_sys (const sigset_t *mask);

extern asmlinkage int sigwait_sys (const sigset_t *set, int *sig);

extern asmlinkage int sigpending_sys (sigset_t *set);

extern asmlinkage int pthread_kill_sys (pthread_t thread, int sig);

extern asmlinkage int pthread_sigmask_sys (int how, const sigset_t *set, 
					   sigset_t *oset);
#endif
/*
extern asmlinkage int sigprocmask_sys (int how, const sigset_t *set, sigset_t *oset); //  not implemented yet

extern int sigpending(sigset_t *set); //  not implemented yet
extern int sigsuspend(const sigset_t *mask); // not implemented yet

extern int sigwait(const sigset_t *set, int *sig);
extern int sigwaitinfo(const sigset_t *set, siginfo_t *info);

//int sigtimedwait(const sigset_t *set, siginfo_t *info,
//               const struct timespec *timeout); (not implemented yet)

extern int sigqueue(int pid, int signo, const union sigval value);

//unsigned int alarm(unsigned int seconds); (not implemented yet)

//int pause(void) (not implemented yet)

// void (*signal(int sig, void (*func)(int)))(int); // (not implemented yet)

//int raise(int sig); (not implemented yet)
*/
extern int init_signals (void);

#endif
