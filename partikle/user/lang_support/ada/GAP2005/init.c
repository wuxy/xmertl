/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                                 I N I T                                  *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *          Copyright (C) 1992-2005, Free Software Foundation, Inc.         *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 2,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License *
 * for  more details.  You should have  received  a copy of the GNU General *
 * Public License  distributed with GNAT;  see file COPYING.  If not, write *
 * to  the Free Software Foundation,  59 Temple Place - Suite 330,  Boston, *
 * MA 02111-1307, USA.                                                      *
 *                                                                          *
--
--
--
--
--
--
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

/*  This unit contains initialization circuits that are system dependent. A
    major part of the functionality involved involves stack overflow checking.
    The GCC backend generates probe instructions to test for stack overflow.
    For details on the exact approach used to generate these probes, see the
    "Using and Porting GCC" manual, in particular the "Stack Checking" section
    and the subsection "Specifying How Stack Checking is Done". The handlers
    installed by this file are used to handle resulting signals that come
    from these probes failing (i.e. touching protected pages) */

/* This file should be kept synchronized with 2sinit.ads, 2sinit.adb, and
   5zinit.adb. All these files implement the required functionality for
   different targets. */

/* The following include is here to meet the published VxWorks requirement
   that the __vxworks header appear before any other include. */

#include "adaint.h"
#include "raise.h"

extern void __gnat_raise_program_error (const char *, int);

/* Addresses of exception data blocks for predefined exceptions. */
extern struct Exception_Data constraint_error;
extern struct Exception_Data numeric_error;
extern struct Exception_Data program_error;
extern struct Exception_Data storage_error;
extern struct Exception_Data tasking_error;
extern struct Exception_Data _abort_signal;

#define Lock_Task system__soft_links__lock_task
extern void (*Lock_Task) (void);

#define Unlock_Task system__soft_links__unlock_task
extern void (*Unlock_Task) (void);

#define Get_Machine_State_Addr \
                      system__soft_links__get_machine_state_addr
extern struct Machine_State *(*Get_Machine_State_Addr) (void);

#define Check_Abort_Status     \
                      system__soft_links__check_abort_status
extern int (*Check_Abort_Status) (void);

#define Raise_From_Signal_Handler \
                      ada__exceptions__raise_from_signal_handler
extern void Raise_From_Signal_Handler (struct Exception_Data *, const char *);

#define Propagate_Signal_Exception \
                      __gnat_propagate_sig_exc
extern void Propagate_Signal_Exception (struct Machine_State *,
                                        struct Exception_Data *,
                                        const char *);

/* Copies of global values computed by the binder */
int   __gl_main_priority            = -1;
int   __gl_time_slice_val           = -1;
char  __gl_wc_encoding              = 'n';
char  __gl_locking_policy           = ' ';
char  __gl_queuing_policy           = ' ';
char  __gl_task_dispatching_policy  = ' ';
char *__gl_restrictions             = 0;
char *__gl_interrupt_states         = 0;
int   __gl_num_interrupt_states     = 0;
int   __gl_unreserve_all_interrupts = 0;
int   __gl_exception_tracebacks     = 0;
int   __gl_zero_cost_exceptions     = 0;
int   __gl_detect_blocking          = 0;

/* Indication of whether synchronous signal handler has already been
   installed by a previous call to adainit */
int  __gnat_handler_installed      = 0;

/* HAVE_GNAT_INIT_FLOAT must be set on every targets where a __gnat_init_float
   is defined. If this is not set them a void implementation will be defined
   at the end of this unit. */
#undef HAVE_GNAT_INIT_FLOAT

/******************************/
/* __gnat_get_interrupt_state */
/******************************/

char __gnat_get_interrupt_state (int);

/* This routine is called from the runtime as needed to determine the state
   of an interrupt, as set by an Interrupt_State pragma appearing anywhere
   in the current partition. The input argument is the interrupt number,
   and the result is one of the following:

       'n'   this interrupt not set by any Interrupt_State pragma
       'u'   Interrupt_State pragma set state to User
       'r'   Interrupt_State pragma set state to Runtime
       's'   Interrupt_State pragma set state to System */

char
__gnat_get_interrupt_state (int intrup)
{
  if (intrup >= __gl_num_interrupt_states)
    return 'n';
  else
    return __gl_interrupt_states [intrup];
}

/**********************/
/* __gnat_set_globals */
/**********************/

/* This routine is called from the binder generated main program.  It copies
   the values for global quantities computed by the binder into the following
   global locations. The reason that we go through this copy, rather than just
   define the global locations in the binder generated file, is that they are
   referenced from the runtime, which may be in a shared library, and the
   binder file is not in the shared library. Global references across library
   boundaries like this are not handled correctly in all systems.  */

/* For detailed description of the parameters to this routine, see the
   section titled Run-Time Globals in package Bindgen (bindgen.adb) */

void
__gnat_set_globals (int main_priority,
                    int time_slice_val,
                    char wc_encoding,
                    char locking_policy,
                    char queuing_policy,
                    char task_dispatching_policy,
                    char *restrictions,
                    char *interrupt_states,
                    int num_interrupt_states,
                    int unreserve_all_interrupts,
                    int exception_tracebacks,
                    int zero_cost_exceptions,
                    int detect_blocking)
{
  static int already_called = 0;

  /* If this procedure has been already called once, check that the
     arguments in this call are consistent with the ones in the previous
     calls. Otherwise, raise a Program_Error exception.

     We do not check for consistency of the wide character encoding
     method. This default affects only Wide_Text_IO where no explicit
     coding method is given, and there is no particular reason to let
     this default be affected by the source representation of a library
     in any case.

     We do not check either for the consistency of exception tracebacks,
     because exception tracebacks are not normally set in Stand-Alone
     libraries. If a library or the main program set the exception
     tracebacks, then they are never reset afterwards (see below).

     The value of main_priority is meaningful only when we are invoked
     from the main program elaboration routine of an Ada application.
     Checking the consistency of this parameter should therefore not be
     done. Since it is assured that the main program elaboration will
     always invoke this procedure before any library elaboration
     routine, only the value of main_priority during the first call
     should be taken into account and all the subsequent ones should be
     ignored. Note that the case where the main program is not written
     in Ada is also properly handled, since the default value will then
     be used for this parameter.

     For identical reasons, the consistency of time_slice_val should not
     be checked. */

  if (already_called)
    {
      if (__gl_locking_policy		   != locking_policy
	  || __gl_queuing_policy           != queuing_policy
	  || __gl_task_dispatching_policy  != task_dispatching_policy
	  || __gl_unreserve_all_interrupts != unreserve_all_interrupts
	  || __gl_zero_cost_exceptions     != zero_cost_exceptions)
	__gnat_raise_program_error (__FILE__, __LINE__);

      /* If either a library or the main program set the exception traceback
         flag, it is never reset later */

      if (exception_tracebacks != 0)
         __gl_exception_tracebacks = exception_tracebacks;

      return;
    }
  already_called = 1;

  __gl_main_priority            = main_priority;
  __gl_time_slice_val           = time_slice_val;
  __gl_wc_encoding              = wc_encoding;
  __gl_locking_policy           = locking_policy;
  __gl_queuing_policy           = queuing_policy;
  __gl_restrictions             = restrictions;
  __gl_interrupt_states         = interrupt_states;
  __gl_num_interrupt_states     = num_interrupt_states;
  __gl_task_dispatching_policy  = task_dispatching_policy;
  __gl_unreserve_all_interrupts = unreserve_all_interrupts;
  __gl_exception_tracebacks     = exception_tracebacks;
  __gl_detect_blocking          = detect_blocking;

  /* ??? __gl_zero_cost_exceptions is new in 3.15 and is referenced from
     a-except.adb, which is also part of the compiler sources. Since the
     compiler is built with an older release of GNAT, the call generated by
     the old binder to this function does not provide any value for the
     corresponding argument, so the global has to be initialized in some
     reasonable other way. This could be removed as soon as the next major
     release is out.  */

  __gl_zero_cost_exceptions = 0;
  /* We never build the compiler to run in ZCX mode currently anyway.  */
}

/* Notes on the Zero Cost Exceptions scheme and its impact on the signal
   handlers implemented below :

   What we call Zero Cost Exceptions is implemented using the GCC eh
   circuitry, even if the underlying implementation is setjmp/longjmp
   based. In any case ...

   The GCC unwinder expects to be dealing with call return addresses, since
   this is the "nominal" case of what we retrieve while unwinding a regular
   call chain. To evaluate if a handler applies at some point in this chain,
   the propagation engine needs to determine what region the corresponding
   call instruction pertains to. The return address may not be attached to the
   same region as the call, so the unwinder unconditionally substracts "some"
   amount to the return addresses it gets to search the region tables. The
   exact amount is computed to ensure that the resulting address is inside the
   call instruction, and is thus target dependant (think about delay slots for
   instance).

   When we raise an exception from a signal handler, e.g. to transform a
   SIGSEGV into Storage_Error, things need to appear as if the signal handler
   had been "called" by the instruction which triggered the signal, so that
   exception handlers that apply there are considered. What the unwinder will
   retrieve as the return address from the signal handler is what it will find
   as the faulting instruction address in the corresponding signal context
   pushed by the kernel. Leaving this address untouched may loose, because if
   the triggering instruction happens to be the very first of a region, the
   later adjustements performed by the unwinder would yield an address outside
   that region. We need to compensate for those adjustments at some point,
   which we currently do in the GCC unwinding fallback macro.

   The thread at http://gcc.gnu.org/ml/gcc-patches/2004-05/msg00343.html
   describes a couple of issues with our current approach. Basically: on some
   targets the adjustment to apply depends on the triggering signal, which is
   not easily accessible from the macro, and we actually do not tackle this as
   of today. Besides, other languages, e.g. Java, deal with this by performing
   the adjustment in the signal handler before the raise, so our adjustments
   may break those front-ends.

   To have it all right, we should either find a way to deal with the signal
   variants from the macro and convert Java on all targets (ugh), or remove
   our macro adjustments and update our signal handlers a-la-java way.  The
   latter option appears the simplest, although some targets have their share
   of subtleties to account for.  See for instance the syscall(SYS_sigaction)
   story in libjava/include/i386-signal.h.  */

/* For all other versions of GNAT, the handler does nothing */

/*******************/
/* Default Section */
/*******************/

void
__gnat_install_handler (void)
{
  __gnat_handler_installed = 1;
}

/*********************/
/* __gnat_init_float */
/*********************/

/* This routine is called as each process thread is created, for possible
   initialization of the FP processor. This version is used under INTERIX,
   WIN32 and could be used under OS/2 */


#ifndef HAVE_GNAT_INIT_FLOAT

/* All targets without a specific __gnat_init_float will use an empty one */
void
__gnat_init_float (void)
{
}
#endif

/***********************************/
/* __gnat_adjust_context_for_raise */
/***********************************/

#ifndef HAVE_GNAT_ADJUST_CONTEXT_FOR_RAISE

/* All targets without a specific version will use an empty one */

/* UCONTEXT is a pointer to a context structure received by a signal handler
   about to propagate an exception. Adjust it to compensate the fact that the
   generic unwinder thinks the corresponding PC is a call return address.  */

void
__gnat_adjust_context_for_raise (int signo,
				 void *ucontext)
{
  /* The point is that the interrupted context PC typically is the address
     that we should search an EH region for, which is different from the call
     return address case. The target independant part of the GCC unwinder
     don't differentiate the two situations, so we compensate here for the
     adjustments it will blindly make.

     signo is passed because on some targets for some signals the PC in
     context points to the instruction after the faulting one, in which case
     the unwinder adjustment is still desired.  */

  /* On a number of targets, we have arranged for the adjustment to be
     performed by the MD_FALLBACK_FRAME_STATE circuitry, so we don't provide a
     specific instance of this routine.  The MD_FALLBACK doesn't have access
     to the signal number, though, so the compensation is systematic there and
     might be wrong in some cases.  */

  /* Having the compensation wrong leads to potential failures.  A very
     typical case is what happens when there is no compensation and a signal
     triggers for the first instruction in a region : the unwinder adjustment
     has it search in the wrong EH region.  */
}

#endif
