/*
 * $FILE: irqs.c
 *
 * Hardware-dependent part of irqs
 * Linux version
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

#include <irqs.h>
#include <bitmap.h>
#include <string.h>
#include <arch/linux_syscalls.h>
#include <arch/processor.h>
#include <processor.h>

static bitmap_t pending_irqs, masked_irqs;
int bit_irqs = 0; // By default, interrupts are disable

//-------------------//
// linux_disable_irq //
//-------------------//

static void linux_disable_irq(unsigned int irq) {
  masked_irqs = setbit (masked_irqs, irq);
}

//------------------//
// linux_enable_irq //
//------------------//

extern void deliver_irqs (void);
static void linux_enable_irq(unsigned int irq) {
  masked_irqs = clearbit (masked_irqs, irq);
  deliver_irqs ();
  //do_signal();
}

//-------------------//
// signal_handler_wr //
//-------------------//

extern void pending_irq (int irq);
extern void do_signals(void);
extern void scheduling (void);
static void signal_handler_wr (int sig) {
  linux_sigset_t sigset;

  irq_nesting_counter ++;
  linux_sigfillset (&sigset);
  linux_sigprocmask (LINUX_SIG_UNBLOCK, &sigset, 0);
  pending_irq (sig);
  deliver_irqs ();
  irq_nesting_counter --;
  if (irq_nesting_counter == SCHED_PENDING) 
    scheduling ();
  do_signals();
}

//-----------------//
// segv_handler_wr //
//-----------------//

static void segv_handler_wr (int sig, linux_siginfo_t *info, void *_ctx) {
  panic (0, "SIGSEGV signal triggered -> Fault address: 0x%x\n", 
	 info->_sifields._sigfault._addr);
}

//---------------//
// init_arch_irq //
//---------------//

int init_arch_irqs (void) {
  struct linux_sigaction sact, act;
  int irq;
  pending_irqs = clearbitmap (); // No pending irqs
  masked_irqs = setbitmap (); // All irqs are masked
  
  for (irq = 0; irq < IRQ_NR; irq ++) {
    hw_irq_ctrl [irq] = (hw_irq_ctrl_t){
      .enable = linux_enable_irq,
      .disable = linux_disable_irq,
      .ack = linux_disable_irq,
      .end = linux_enable_irq
    };
  }
  memset (&sact.sa_mask, 0, sizeof (linux_sigset_t));
  sact.__sigaction_handler.linux_sa_handler = signal_handler_wr;
  sact.sa_flags = LINUX_SA_RESTART;

  // installing some handlers
  // SIGALRM
  linux_sigaction (LINUX_SIGALARM, &sact, NULL);
  
  act.__sigaction_handler.linux_sa_sigaction = segv_handler_wr;
  linux_sigfillset (&act.sa_mask);
  act.sa_flags = LINUX_SA_SIGINFO; // SA_SIGINFO
  linux_sigaction (LINUX_SIGSEGV, &act, 0); // SIGSEGV
  linux_sigaction (LINUX_SIGBUS, &act, 0); // SIGBUS

  return 0;
}

// Non-exportable functions
// Useful only for the linux-i386 arch

//-------------//
// pending_irq //
//-------------//

void pending_irq(int irq) {
  pending_irqs = setbit (pending_irqs, irq);
}

//-------------//
// deliver_irq //
//-------------//

static context_t default_context;

// Called internally in the linux architecture to send a interrupt.
// This is a non-portable function
void deliver_irqs (void) {
  int irq, flags;

  if (!bit_irqs) return;

  for (irq = 0; irq < IRQ_NR; irq ++) {
    if (isbitset (pending_irqs, irq) && !isbitset (masked_irqs, irq)) {
      // no disable-irq function can be called from here, since it
      // would provoke a recursive never-ending behaviour
      pending_irqs = clearbit (pending_irqs, irq);
      flags = bit_irqs;
      bit_irqs = 0;
      default_context.irqnr = irq;
      default_context.error_code = 0;
      // Calling the handler
      ((irq_handler_t)(*irq_handler_table [irq]))(&default_context);
      bit_irqs = flags;
    }
  }
  
}
