/*
 * $FILE: signal.c
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

/*
 * Fixed various bugs in signal handler mask - Vicent Brocal
 */
 
#include <config.h>
#include <bitop.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <arch/processor.h>

struct signal_table_struct signal_table [NRSIGNALS];

//--------------------//
// sa_handler_SIG_DFL //
//--------------------//

void sa_handler_SIG_DFL (int signal) {
  /* 
   * Finishing with the whole application, the guilt is a signal
   */
  exit_sys (-1);
}

//--------------------//
// sa_handler_SIG_IGN //
//--------------------//

void sa_handler_SIG_IGN (int signal) {
  /* Doing nothing */
}

//-----------------------//
// WAKEUP_THREAD_handler //
//-----------------------//

static void WAKEUP_THREAD_handler (int signal) {
  // When the thread receive one of this signal it automatically wakes
  // up so we have to do nothing
}

//----------------------//
// SLEEP_THREAD_handler //
//----------------------//

static void SLEEP_THREAD_handler (int signal) {
  suspend_thread (current_thread);
  scheduling ();
}

//-----------------------//
// CANCEL_THREAD_handler //
//-----------------------//

static void CANCEL_THREAD_handler (int signal) {
  // Canceling this thread
  pthread_exit_sys ((void *) 0);
}

//---------------//
// sigaction_sys //
//---------------//

asmlinkage int sigaction_sys (int sig, const struct sigaction *act,
			      struct sigaction *oact) {
  if (sig < FIRST_USER_SIGNAL || sig >= NRSIGNALS) {
    SET_ERRNO (EINTR);
    return -1;
  }

  if (oact) {
    if (signal_table [sig].sa_handler == sa_handler_SIG_DFL)
      oact -> sa_handler = SIG_DFL;
    else
      if (signal_table [sig].sa_handler == sa_handler_SIG_IGN)
	oact -> sa_handler = SIG_IGN;
      else
	oact -> sa_handler = signal_table [sig].sa_handler;
    oact -> sa_mask = signal_table [sig].sa_mask;
  }

  if (act) {
    if (act -> sa_handler == SIG_DFL) 
      signal_table [sig].sa_handler = sa_handler_SIG_DFL;
    else
      if (act -> sa_handler == SIG_IGN)
	signal_table [sig].sa_handler = sa_handler_SIG_IGN;
      else 
	signal_table [sig].sa_handler = act -> sa_handler;

    signal_table [sig].sa_mask = act -> sa_mask;
  }
  return 0;
}

//------------------//
// pthread_kill_sys //
//------------------//

asmlinkage int pthread_kill_sys (pthread_t thread, int sig) {
  if (!thread || thread -> magic_number != PTHREAD_MAGIC_NUMBER) {
    SET_ERRNO (ESRCH);
    return -1;
  }

  if (sig <  0 || sig >= NRSIGNALS) {
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  thread -> sigpending.sig |= (1 << sig);
  /* 
   * If the signal isn't blocked, the thread is woken up
   */

  if ((~(thread -> sigmask.sig & ~non_maskable_signals) & 
      thread -> sigpending.sig) || GET_THREAD_ACTIVATE_ON_SIGNAL(thread)) 
    activate_thread (thread);
    
  return 0;
}

//---------------------//
// pthread_sigmask_sys //
//---------------------//

asmlinkage int pthread_sigmask_sys (int how, const sigset_t *set, 
				    sigset_t *oset) {
  if (oset)
    oset -> sig = current_thread -> sigmask.sig;

  if (set)
    switch (how) {
    case SIG_BLOCK:
      current_thread -> sigmask.sig |= set -> sig;
      break;
    case SIG_SETMASK:
      current_thread -> sigmask.sig = set -> sig;
      break;
    case SIG_UNBLOCK:
      current_thread -> sigmask.sig &= ~(set -> sig);
      break;
    default:
      SET_ERRNO (EINVAL);
      return -1;
    }
  
  return 0;
}

//----------------//
// sigsuspend_sys //
//----------------//

asmlinkage int sigsuspend_sys (const sigset_t *mask) {
  sigset_t old_mask = current_thread -> sigmask;
  int flags;

  hw_save_flags_and_cli (flags);

  if (mask)
    current_thread -> sigmask = *mask;

  suspend_thread (current_thread);
  scheduling ();
  current_thread -> sigmask = old_mask;
  
  hw_restore_flags (flags);

  return 0;
}

//-------------//
// sigwait_sys //
//-------------//

asmlinkage int sigwait_sys (const sigset_t *set, int *sig) {
  int flags, pending;

  if (!set) {
    SET_ERRNO (EINVAL);
    return -1;
  }
  
  hw_save_flags_and_cli (flags);
  pending = (current_thread -> sigpending.sig & 
	     (set -> sig & ~non_maskable_signals));
  if (pending) {
    pending = _ffs (pending);
    if (sig)
      *sig = pending;
    current_thread -> sigpending.sig &= ~(1<<pending);
    hw_restore_flags (flags);
    return 0;
  }
  
  SET_THREAD_ACTIVATE_ON_SIGNAL(current_thread, 1);
  sigsuspend_sys (set);
  SET_THREAD_ACTIVATE_ON_SIGNAL(current_thread, 0);
  pending = (current_thread -> sigpending.sig & 
	     (set -> sig & ~non_maskable_signals));
  if (pending) {
    pending = _ffs (pending);
    if (sig)
      *sig = pending;
    current_thread -> sigpending.sig &= ~(1<<pending);
  } else {
    hw_restore_flags (flags);
    SET_ERRNO (EINVAL);
    return -1;
  }

  hw_restore_flags (flags);

  return 0;
}

//----------------//
// sigpending_sys //
//----------------//

asmlinkage int sigpending_sys (sigset_t *set) {
  if (set) *set = current_thread -> sigpending;

  return 0;
}

//--------------//
// init_signals //
//--------------//

int init_signals (void) {
  int signal;
  memset ((char *)&signal_table, 0, 
	  sizeof (struct signal_table_struct) * NRSIGNALS);
  
  for (signal = 0; signal < NRSIGNALS; signal ++) {
    signal_table [signal].sa_handler = sa_handler_SIG_IGN;
    sigaddset (&signal_table [signal].sa_mask, signal);
  }

  signal_table [CANCEL_SIGNAL].sa_handler = CANCEL_THREAD_handler;
  signal_table [SLEEP_SIGNAL].sa_handler = SLEEP_THREAD_handler;
  signal_table [WAKEUP_SIGNAL].sa_handler = WAKEUP_THREAD_handler;
  return 0;
}

//------------//
// do_signals //
//------------//

extern unsigned long _stext, _etext;
void do_signals (void) {
  int flags, signal, p1 = 0, p2 = 0;
  unsigned long sigpend, old_sigmask;

  hw_save_flags_and_cli (flags);
  /*
   * Executing pending signals
   */
  sigpend = (current_thread -> sigpending.sig 
	     & ~(current_thread -> sigmask.sig & ~non_maskable_signals));

  if (sigpend || current_thread -> sigpending.sig)
    current_thread -> sigpending.sig &= ~sigpend;

  /* 
   * Looking at the current thread canceability state
   */
  if (GET_THREAD_CANCEL_STATE(current_thread)) {
    current_thread -> sigpending.sig |= (sigpend & (1 << CANCEL_SIGNAL));
    sigpend &= ~(1 << CANCEL_SIGNAL);
  }

  old_sigmask = current_thread -> sigmask.sig;

  for (signal = 0; signal < NRSIGNALS && sigpend; signal ++) {
    current_thread -> sigmask.sig = old_sigmask | signal_table [signal].sa_mask.sig | (1 << signal);
    if ((sigpend & 1)) {
      hw_sti ();
      // Excuting the handler associated with this signal
      if ((unsigned long)signal_table [signal].sa_sigaction > 
	  (unsigned long)&_stext && 
	  (unsigned long)signal_table [signal].sa_sigaction < 
	  (unsigned long)&_etext) {
	// internal handler
	if (signal_table [signal].sa_sigaction)
	  signal_table [signal].sa_sigaction (signal, 
					      (siginfo_t *) p1, (void *) p2);
      } else {
	// external handler
	if (signal_table [signal].sa_sigaction) {
	  CALL_SIGNAL_HANDLER 
	    (signal_table [signal].sa_sigaction, signal, 
	     (siginfo_t *)p1, (void *)p2);
	}
      }
      hw_cli ();
    }
    sigpend >>= 1;
  }

  current_thread -> sigmask.sig = old_sigmask;
  hw_restore_flags (flags);
}
