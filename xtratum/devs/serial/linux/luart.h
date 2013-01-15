/*
 * $FILE:
 * 	luart.h
 *
 * Description:
 * 	linux uart device driver header file
 *
 * Author
 * 	Bai Shuwei<baishuwei@dslab.lzu.edu.cn>
 *
 * Date
 * 	Sep. 7, 2008
 * */

#ifndef __LINUX_UART_H__
#define __LINUX_UART_H__

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


typedef struct 
uart_dev_struct uart_dev_t;

struct uart_dev_struct
{
	int irq;
	int base;
	int parity;
	int speed;
};

#define CAS(adr, ov, nv) ({ \
	__typeof__(ov) ret; \
	__asm__ __volatile__( \
	"cmpxchg %3,%1" \
	:"=a"(ret),"+m"(*(volatile unsigned long *)(adr)) \
	:"a"(ov),"r"(nv)); \
	ret; \
	})

#define is_bit_set(flag, index) (flag & (1 << index))
#define set_bit(flag, index) (flag |= 1 << index)

#endif
