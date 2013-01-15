/*
 * $FILE: pl011.c
 *
 * PrimeCell UART (PL011)
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
 
#include <config.h>
#include <arch/drivers.h>
#include <arch/io.h>
#include "pl011.h"

#define CONSOLE_PORT CONFIG_CONS_INDEX
#define baudRate CONFIG_BAUDRATE
static volatile unsigned char *const port[] = CONFIG_PL01x_PORTS;

#define NUM_PORTS (sizeof(port)/sizeof(port[0]))

//------------//
// init_pl011 //
//------------//

int init_pl011 (void) {
  unsigned int temp;
  unsigned int divider;
  unsigned int remainder;
  unsigned int fraction;
  
  /* Disabling everything */
  io_write (port[CONSOLE_PORT] + UART_PL011_CR, 0x0);
  
  /* Set baud rate*/
  /* IBRD = UART_CLK / (16 * BAUD_RATE) */
  /* FBRD = ROUND((64 * MOD(UART_CLK,(16 * BAUD_RATE))) / (16 *
     BAUD_RATE)) */

  temp = 16 * baudRate;
  divider = CONFIG_PL011_CLOCK / temp;
  remainder = CONFIG_PL011_CLOCK % temp;
  temp = (8 * remainder) / baudRate;
  fraction = (temp >> 1) + (temp & 1);
  
  io_write (port[CONSOLE_PORT] + UART_PL011_IBRD, divider);
  io_write (port[CONSOLE_PORT] + UART_PL011_FBRD, fraction);
  
  /* Set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled. */
  io_write (port[CONSOLE_PORT] + UART_PL011_LCRH,
	    (UART_PL011_LCRH_WLEN_8 | UART_PL011_LCRH_FEN));

  /* Enabling the UART */
  io_write (port[CONSOLE_PORT] + UART_PL011_CR,
	    (UART_PL011_CR_UARTEN | UART_PL011_CR_TXE |
	     UART_PL011_CR_RXE));
  
  return 0;
}

//------------//
// pl011_putc //
//------------//
static void pl011_putc (int portnum, char c) {
  /* Wait until there is space in the FIFO */
  while (io_read (port[portnum] + UART_PL01x_FR) & UART_PL01x_FR_TXFF);

  /* Send the character */
  io_write (port[portnum] + UART_PL01x_DR, c);
}

//------------//
// pl011_getc //
//------------//
static int pl011_getc (int portnum) {
  unsigned int data;
  
  // Wait until there is data in the FIFO
  while (io_read (port[portnum] + UART_PL01x_FR) & UART_PL01x_FR_RXFE);
  
  data = io_read (port[portnum] + UART_PL01x_DR);
  
  // Check for an error flag
  if (data & 0xFFFFFF00) {
    // Clear the error 
    io_write (port[portnum] + UART_PL01x_ECR, 0xFFFFFFFF);
    return -1;
  }
  
  return (int) data;
}

//------------//
// pl011_tstc //
//------------//
/*
static int pl011_tstc (int portnum) {
  return !(io_read (port[portnum] + UART_PL01x_FR) &
	   UART_PL01x_FR_RXFE);
}
*/
//-------------//
// serial_putc //
//-------------//

static void serial_putc (const char c) {
  if (c == '\n')
    pl011_putc (CONSOLE_PORT, '\r');
  
  pl011_putc (CONSOLE_PORT, c);
}

//-------------//
// serial_puts //
//-------------//

int serial_puts (const char *str, unsigned long length) {
  int n;

  for (n = 0; n < length; n++)
    serial_putc (str[n]);

  return length;
}

//-------------//
// serial_getc //
//-------------//

int serial_getc (void) {
  return pl011_getc (CONSOLE_PORT);
}

//-------------//
// serial_tstc //
//-------------//
/*
static int serial_tstc (void) {
  return pl011_tstc (CONSOLE_PORT);
}
*/
//---------------//
// serial_setbrg //
//---------------//
/*
static void serial_setbrg (void) {
}
*/

