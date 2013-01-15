/*
 * $FILE: 16550A.h
 *
 * Description
 *
 * Author: Bai Shuwei <baishuwei@dslab.lzu.edu.cn>
 * */

#include <xm/events.h>

#ifndef __XM_16550A_H__

#define __XM_16550A_H__

extern event_handling_t *event_handling;

#define MAX_UART_CLIENT 32

#define TTYS0_IRQ 3
#define TTYS1_IRQ 4

#define Serial_PORT1_BASE (0x3f8)
#define Serial_PORT2_BASE (0x2f8)

#define BASE Serial_PORT1_BASE

#define RBR (BASE + 0)
#define THR RBR
#define DLL RBR
#define IER (BASE + 1)
#define DLM IER
#define IIR (BASE + 2)
#define FCR IIR
#define LCR (BASE + 3)
#define MCR (BASE + 4)
#define LSR (BASE + 5)
#define MSR (BASE + 6)
#define SCR (BASE + 7)

#if 0 /* line status register */
0	Data available
1	Overrun error
2	Parity error
3	Framing error
4	Break signal received
5	THR is empty
6	THR is empty, and line is idle
7	Errornous data in FIFO
#endif 
#define LSR_DATAA	0x01
#define LSR_OVERE	0x02
#define LSR_PARIE	0x04
#define LSR_FRAME	0x08
#define LSR_BSGRE	0x10
#define LSR_THREE	0x20
#define LSR_TELIE	0x40
#define LSR_EDIFE	0x80

#define IIR_NIP		0x01
#define IIR_THRE	0x02
#define IIR_RDA		0x04
#define IIR_LSC		0x06
#define IIR_MSC		0x00
#define IIR_MSK		0x0F

#define MSR_RD		0x20
#define MSR_TM		0x10

#define UART_PACK_SIZE 128 
#define UART_DATA_SIZE 127

#define INVALID 0xFF

typedef unsigned char uint8_t;

typedef struct
uart_package_struct 
{
	int size;
	int indx;
	union uart_pack {
		struct {
			uint8_t id;
			char data[UART_DATA_SIZE];
		} spack;
		uint8_t upack[UART_PACK_SIZE];
	} pack;
} uart_pack_t;

#define uart_io_read(addr) inb(addr)
#define uart_io_write(u8, addr) outb(u8, addr)
#endif
