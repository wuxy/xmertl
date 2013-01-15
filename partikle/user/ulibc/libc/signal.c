/*
 * $FILE: signal.c
 *
 * Posix threads system calls
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

#include <config.h>
#include <errno.h>
#include <signal.h>
#include <syscalls.h>

//-----------//
// sigaction //
//-----------//

_syscall3 (int, sigaction, int, sig, const struct sigaction *, act,
	   struct sigaction *, oact);

//--------------//
// pthread_kill //
//--------------//

_syscall2 (int, pthread_kill, pthread_t, thread, int, sig);

//-----------------//
// pthread_sigmask //
//-----------------//

_syscall3 (int, pthread_sigmask, int, how, const sigset_t *, set, 
	   sigset_t *, oset);

//---------//
// sigwait //
//---------//

_syscall2 (int, sigwait, const sigset_t *, set, int *, sig);

//------------//
// sigsuspend //
//------------//

_syscall1 (int, sigsuspend, const sigset_t *, mask);

//------------//
// sigpending //
//------------//

_syscall1 (int, sigpending, sigset_t *, set);

//-------------//
// sigemptyset //
//-------------//

int sigemptyset(sigset_t *set) {
  if (set)
    set -> sig = 0;
  else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  return 0;
}

//------------//
// sigfillset //
//------------//

int sigfillset(sigset_t *set) {
  if (set)
    set -> sig = ~0;
  else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  return 0;
}

//-----------//
// sigaddset //
//-----------//

int sigaddset(sigset_t *set, int signo) {
  if (set)
    set -> sig |= (1 << signo);
  else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  return 0;
}

//-----------//
// sigdelset //
//-----------//

int sigdelset(sigset_t *set, int signo) {

  if (set)
    set -> sig &= ~(1 << signo);
  else {
    SET_ERRNO (EINVAL);
    return -1;
  }

  return 0;
}

//-------------//
// sigismember //
//-------------//

int sigismember(const sigset_t *set, int signo) {
  unsigned long aux_set;
  if (set) {
    aux_set = set -> sig & (1 << signo);
  } else {
    SET_ERRNO (EINVAL);
    return -1;
  }
  return ((aux_set)?1:0);
}
