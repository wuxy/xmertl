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

#ifndef _ULIBC_SIGNAL_H_
#define _ULIBC_SIGNAL_H_

#include <sys/types.h>
#include <kernel_inc/signal.h>
#include <kernel_inc/compiler.h>

BEGIN_DECLS

//extern int kill(int pid, int sig);

extern int sigaction(int sig, const struct sigaction *act,
		     struct sigaction *oact);

extern int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);
//extern int sigprocmask(int how, const sigset_t *set, sigset_t *oset);

extern int sigpending(sigset_t *set);
extern int sigsuspend(const sigset_t *mask);

extern int sigwait(const sigset_t *set, int *sig);
//extern int sigwaitinfo(const sigset_t *set, siginfo_t *info);

//int sigtimedwait(const sigset_t *set, siginfo_t *info,
//               const struct timespec *timeout); (not implemented yet)

//extern int sigqueue(int pid, int signo, const union sigval value);

extern int pthread_kill(pthread_t thread, int sig);

//unsigned int alarm(unsigned int seconds); (not implemented yet)

//int pause(void) (not implemented yet)

// void (*signal(int sig, void (*func)(int)))(int); // (not implemented yet)

//int raise(int sig); (not implemented yet)

extern int sigemptyset(sigset_t *set);
extern int sigfillset(sigset_t *set);
extern int sigaddset(sigset_t *set, int signo);
extern int sigdelset(sigset_t *set, int signo);
extern int sigismember(const sigset_t *set, int signo);

END_DECLS

#endif
