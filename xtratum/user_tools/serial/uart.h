/*
 * $FILE: uart.h
 *
 * Description
 *
 *
 * Author: Bai Shuwei<baishuwei@dslab.lzu.edu.cn>
 *
 * */

#ifndef __XM_DOM_UART_H__
#define __XM_DOM_UART_H__

typedef unsigned char uint8_t;

#define VUART_SERIAL_COUNT 16

#define UART_RBR_OFFSET 0 /* Read, receiver buffer */
#define UART_THR_OFFSET 0 /* Write, transmitter holding */
#define UART_DLL_OFFSET 0 /* Read/Write, divisor latch LSB */
#define UART_IER_OFFSET 1 /* Read/Write, interrupt enable */
#define UART_DLM_OFFSET 1 /* Read/Write, divisor latch MSB */
#define UART_IIR_OFFSET 2 /* Read, interrupt identification */ 
#define UART_FCR_OFFSET 2 /* Write, FIFO control */
#define UART_LCR_OFFSET 3 /* line control */
#define UART_MCR_OFFSET 4 /* modem control */
#define UART_LSR_OFFSET 5 /* Read, line status*/
#define UART_MSR_OFFSET 6 /* Modem status */
#define UART_SCR_OFFSET 7 /* scrach */

#define UART_DLAB_ENABLE 0x80

#define UART_BAUT_CMD 0x1

#define UART_PACK_SIZE 60

#define UART_RBR(uart) (uart.base+UART_RBR_OFFSET) 
#define UART_THR(uart) (uart.base+UART_THR_OFFSET)
#define UART_DLL(uart) (uart.base+UART_DLL_OFFSET)  /* Read/Write, divisor latch LSB */
#define UART_IER(uart) (uart.base+UART_IER_OFFSET)  /* Read/Write, interrupt enable */
#define UART_DLM(uart) (uart.base+UART_DLM_OFFSET)  /* Read/Write, divisor latch MSB */
#define UART_IIR(uart) (uart.base+UART_IIR_OFFSET)  /* Read, interrupt identification */ 
#define UART_FCR(uart) (uart.base+UART_FCR_OFFSET)  /* Write, FIFO control */
#define UART_LCR(uart) (uart.base+UART_LCR_OFFSET)  /* line control */
#define UART_MCR(uart) (uart.base+UART_MCR_OFFSET)  /* modem control */
#define UART_LSR(uart) (uart.base+UART_LSR_OFFSET)  /* Read, line status*/
#define UART_MSR(uart) (uart.base+UART_MSR_OFFSET)  /* Modem status */
#define UART_SCR(uart) (uart.base+UART_SCR_OFFSET)  /* scrach */

/* IIR register */

#define UART_NO_PENDING	0x01
#define UART_MSC_IRQ 	0x00
#define UART_THE_IRQ 	0x02
#define UART_RDA_IRQ 	0x04
#define UART_LSC_IRQ 	0x06
#define UART_FFE_IRQ 	0xC0
#define UART_IIR_MSK	0x0F

/* IER register */
#define UART_RDA_IRQ_ENABLE 0x01
#define UART_THE_IRQ_ENABLE 0x02
#define UART_RLC_IRQ_ENABLE 0x04
#define UART_MSC_IRQ_ENABLE 0x08

/* MSR register */

#define UART_SEND_REQUEST  0x20
#define UART_SEND_TERMINAL 0x10

/* LSR register */
/*
0	Data available
1	Overrun error
2	Parity error
3	Framing error
4	Break signal received
5	THR is empty
6	THR is empty, and line is idle
7	Errornous data in FIFO
*/

#define UART_DATA_ABLE 0x01
#define UART_ORUN_ERR  0x02
#define UART_PRIT_ERR  0x04
#define UART_FRAM_ERR  0x08
#define UART_BKSG_RECV 0x10
#define UART_TRAS_EMPT 0x20
#define UART_LINE_IDEL 0x40
#define UART_ERR_FIFO  0x80

typedef struct 
uart_dev_struct
{
	int irq;
	int base;
	int parity;
	int speed;
} uart_dev_t;

typedef struct
uart_buff_struct
{
	int top;
	int size;
	uint8_t data[UART_PACK_SIZE];
} uart_buff_t;


#define CAS(adr, ov, nv) ({ \
	__typeof__(ov) ret; \
	__asm__ __volatile__( \
	"cmpxchg %3,%1" \
	:"=a"(ret),"+m"(*(volatile unsigned long *)(adr)) \
	:"a"(ov),"r"(nv)); \
	ret; \
	})

#endif

