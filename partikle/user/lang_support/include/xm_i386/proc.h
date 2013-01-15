#ifndef _LANG_SUPPORT_PROC_H_
#define _LANG_SUPPORT_PROC_H_

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.

   In the 80386 we give the 8 general purpose registers the numbers 0-7.
   We number the floating point registers 8-15.
   Note that registers 0-7 can be accessed as a  short or int,
   while only 0-3 may be used with byte `mov' instructions.

   Reg 16 does not correspond to any hardware register, but instead
   appears in the RTL as an argument pointer prior to reload, and is
   eliminated during reloading in favor of either the stack or frame
   pointer.  */

#define FIRST_PSEUDO_REGISTER 53


#endif
