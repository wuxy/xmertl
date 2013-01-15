/* Return backtrace of current program state.
   Copyright (C) 1998, 2000, 2003, 2004, 2005 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1998.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

//#include <bits/libc-lock.h>
//#include <dlfcn.h>
//#include <execinfo.h>
#include <stddef.h>
#include <stdlib.h>
#include <unwind.h>

struct trace_arg
{
  void **array;
  int cnt, size;
  void *lastebp, *lastesp;
};

# define unwind_backtrace _Unwind_Backtrace
# define unwind_getip _Unwind_GetIP
# define unwind_getcfa _Unwind_GetCFA
# define unwind_getgr _Unwind_GetGR

static _Unwind_Reason_Code
backtrace_helper (struct _Unwind_Context *ctx, void *a)
{
  struct trace_arg *arg = a;

  /* We are first called with address in the __backtrace function.
     Skip it.  */
  if (arg->cnt != -1)
    arg->array[arg->cnt] = (void *) unwind_getip (ctx);
  if (++arg->cnt == arg->size)
    return _URC_END_OF_STACK;

  /* %ebp is DWARF2 register 5 on IA-32.  */
  arg->lastebp = (void *) unwind_getgr (ctx, 5);
  arg->lastesp = (void *) unwind_getcfa (ctx);
  return _URC_NO_REASON;
}


/* This is a global variable set at program start time.  It marks the
   highest used stack address.  */
extern void *__libc_stack_end;


/* This is the stack layout we see with every stack frame
   if not compiled without frame pointer.

            +-----------------+        +-----------------+
    %ebp -> | %ebp last frame--------> | %ebp last frame--->...
            |                 |        |                 |
            | return address  |        | return address  |
            +-----------------+        +-----------------+

   First try as far to get as far as possible using
   _Unwind_Backtrace which handles -fomit-frame-pointer
   as well, but requires .eh_frame info.  Then fall back to
   walking the stack manually.  */

struct layout
{
  struct layout *ebp;
  void *ret;
};


int backtrace (void **array, int size) {
  struct trace_arg arg = { .array = array, .size = size, .cnt = -1 };

  if (size >= 1)
    unwind_backtrace (backtrace_helper, &arg);

  if (arg.cnt > 1 && arg.array[arg.cnt - 1] == NULL)
    --arg.cnt;
  else if (arg.cnt < size)
    {
      struct layout *ebp = (struct layout *) arg.lastebp;

      while (arg.cnt < size)
	{
	  /* Check for out of range.  */
	  if ((void *) ebp < arg.lastesp 
	      //|| (void *) ebp > __libc_stack_end
	      || ((long) ebp & 3))
	    break;

	  array[arg.cnt++] = ebp->ret;
	  ebp = ebp->ebp;
	}
    }
  return arg.cnt != -1 ? arg.cnt : 0;
}
