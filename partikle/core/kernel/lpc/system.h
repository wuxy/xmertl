/**************************************************************************
 *
 *   File: armSystem.h
 *   Revision: 1.0
 *   Source file for Philips LPC ARM Processors.
 *   Developed into uBiro project.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/
#ifndef ARM_SYSTEM_H
#define ARM_SYSTEM_H

#include "LPC214x.h"

/**** PLL definitions ****/
#define FOSC                (10000000)  /* Master Oscillator*/
#define PLL_MUL             (6)         /* PLL Multiplier*/
#define CCLK                (FOSC * PLL_MUL) /*CPU Clock*/

/**** Test value range testing ****/
#if ((FOSC < 10000000) || (FOSC > 25000000))
#error Fosc out of range (10MHz-25MHz)
#endif

#if ((CCLK < 10000000) || (CCLK > 60000000))
#error cclk out of range (10MHz-60MHz)
#endif

#define FCCO_MAX      (320000000)       /* Max CC Osc*/
#define PLL_DIV       (FCCO_MAX / (2 * CCLK))  /*PLL Divider*/
#define FCCO          (FOSC * PLL_MUL * 2 * PLL_DIV)

/**** Test value range testing ****/
#if ((FCCO < 150000000) || (FCCO > 320000000))
#error Fcco out of range (156MHz-320MHz)
#endif


/**** Clock for Pheripheral devices ****/

#define VPB_DIVIDER         4           /* 1, 2, or 4  Relationship with the processor clock (CCLK)*/
#define PCLK                (CCLK / VPB_DIVIDER) /* Pheripheal Bus Clock*/

#if ((VPB_DIVIDER != 1) && (VPB_DIVIDER != 2) && (VPB_DIVIDER != 4))
#error VLSI Pheripheal Bus Divider (VPB_DIVIDER) illegal value (1, 2, or 4)
#endif

#define VPBDIV_VALUE  (VPB_DIVIDER & 0x03)     /* VPBDIV value */


/* PLLCON Register */
#define PLLCON_PLLE   (1 << 0)          /* PLL Enable */
#define PLLCON_PLLC   (1 << 1)          /* PLL Connect */

/* PLLCFG Register Bit */
#define PLLCFG_MSEL   ((PLL_MUL - 1) << 0) /* PLL Multiplier */
#define PLLCFG_PSEL   ((PLL_DIV - 1) << 5) /* PLL Divider */

/* PLLSTAT Register Bit */
#define PLLSTAT_LOCK  (1 << 10)        

/***** MAM defines ******/

/*Memory modes*/
#define MAMCR_OFF     0
#define MAMCR_PART    1
#define MAMCR_FULL    2

#define MAMTIM_CYCLES (((CCLK) + 19999999) / 20000000)

/* MEMMAP defines */
#define MEMMAP_BBLK   0                 /* Interrupt Vectors in Boot Block */
#define MEMMAP_FLASH  1                 /* Interrupt Vectors in Flash */
#define MEMMAP_SRAM   2                 /* Interrupt Vectors in SRAM */

/*Initialization of the ARM system*/
int sysinit();

#endif
