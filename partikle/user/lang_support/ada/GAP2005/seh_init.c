/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                              S E H - I N I T                             *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *             Copyright (C) 2005, Free Software Foundation, Inc.           *
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

/*  This unit contains support for SEH (Structured Exception Handling).
    Right now the only implementation is for Win32.  */

////#include "config.h"
////#include "system.h"

#include "raise.h"

/* Addresses of exception data blocks for predefined exceptions. */
extern struct Exception_Data constraint_error;
extern struct Exception_Data numeric_error;
extern struct Exception_Data program_error;
extern struct Exception_Data storage_error;
extern struct Exception_Data tasking_error;
extern struct Exception_Data _abort_signal;

#define Raise_From_Signal_Handler \
                      ada__exceptions__raise_from_signal_handler
extern void Raise_From_Signal_Handler (struct Exception_Data *, const char *);

/* For all non Windows targets we provide a dummy SEH install handler.  */
void __gnat_install_SEH_handler (void *eh)
{
}
