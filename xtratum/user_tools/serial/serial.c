/*
 * $FILE: uart.c
 *
 * Description:
 * 	the uart device driver domain
 *
 * Author: Bai Shuwei<baishuwei@dslab.lzu.edu.cn>
 * */

#include <doms/stdlib.h>
#include <doms/string.h>
#include <doms/errno.h>
#include <devbug.h>
#include <io.h>
#include <xm/serial.h>
#include <xm_syscalls.h>

#include "uart.h"

static rt_serial_t rt_serial[VUART_SERIAL_COUNT];

static uart_dev_t uart = {4, 0x3f8, 0, 115200};
static uart_buff_t uin = {0, UART_PACK_SIZE};
static uart_buff_t uout;

int dom_serial_fifo_write(dom_fifo_t *, uint8_t *, int);
int dom_serial_fifo_read(dom_fifo_t *, uint8_t *, int);

static int print(char *string)
{
	int i = 0;
	while(string[i++]);

	write_scr(string, i);

	return i;
}

#if 0
static inline uint8_t uart_port_read(int port)
{
	uint8_t data;
	data = inb(port);

	return data;
}


static inline void uart_port_write(uint8_t data, int port)
{
	outb(data, port);
}

#else

#define uart_port_read(port) inb(port)
#define uart_port_write(data, port) outb(data, port)

#endif

static uint8_t uart_domain_schedule(void)
{
	uint8_t id;

	uint8_t mid = 0xFF;

	for (id = 0; id < VUART_SERIAL_COUNT; id++) {
		
		if (rt_serial[id].out.serf->top !=
			rt_serial[id].out.serf->bom) {
			mid = id;
			break;
		}
	}

	while (id < VUART_SERIAL_COUNT) {
		
		if( ( rt_serial[id].out.serf->top !=
			rt_serial[id].out.serf->bom ) &&
			(rt_serial[mid].out.serf->prio >
			rt_serial[id].out.serf->prio)) {
			mid = id;
		}

		id ++;
	}

	return mid;
}

static int uart_fill_out_buff(void)
{
	uint8_t vuart, destid;
	int size;

	vuart = uart_domain_schedule();

	if (vuart >= VUART_SERIAL_COUNT) {
		return 0;
	}

	destid = (rt_serial[vuart].out.serf->flag >> 24) & 0xFF;
	uout.data[0] = destid;

	size = dom_serial_fifo_read(&rt_serial[vuart].out, 
			&uout.data[1], UART_PACK_SIZE-1);
	
	uout.size = size + 1;
	uout.top = 0;

	return size;
}
static inline int uart_out_getb(uint8_t *data)
{
	/* all the data have been read */
	if(uout.top >= uout.size) {
		return 0;
	}

	*data = uout.data[uout.top++];

	return 1;
}
static inline int uart_in_putb(uint8_t data)
{
	int val;

	val = 0;

	if (uin.top < uin.size) {
		uin.data[uin.top++] = data;
		val = 1;
	}
	
	return val;
}

static int uart_reset_in_buff(void)
{
	uin.top = 0;
	uin.size = UART_PACK_SIZE;
	
	return 0;
}

static int uart_flush_in_buff(void)
{
	uint8_t id;
	int size = 0;
	
	if (uin.top < 2) return 0;
	/* */
	id = uin.data[0];

	if (id < VUART_SERIAL_COUNT ) {
		size = dom_serial_fifo_write(&rt_serial[id].in, 
					&uin.data[1], uin.top - 1);
	}

	uart_reset_in_buff();

	return size;
}

static inline void uart_recv_start(void)
{
	uart_flush_in_buff();
	uart_reset_in_buff();
	
}

static inline void uart_recv_stop(void)
{
	uart_flush_in_buff();
}

static inline void uart_THE_handler(void)
{
	uint8_t data;
	uint8_t ch;
	int ret;
		
	data = uart_port_read(UART_LSR(uart));

	while (data & UART_TRAS_EMPT) {
		ret = uart_out_getb(&ch);

		/* reading some data in the out buffer */
		if (ret == 1) {
			uart_port_write(ch, UART_THR(uart));
		} else {
		/* no data in the out buffer */
			/* package terminal */
			uart_port_write(0xA, UART_MCR(uart));

			ret = uart_fill_out_buff();

			if (ret == 0) {
				data = uart_port_read(UART_IER(uart));
				data &= ~UART_THE_IRQ_ENABLE;
				uart_port_write(data, UART_IER(uart));
				break;
			} else {
				uart_port_write(0x9, UART_MCR(uart));

				ret = uart_out_getb(&ch);
				if (ret == 1) {
					uart_port_write(ch, UART_THR(uart));
				}
			} 
		}		
		
		data = uart_port_read(UART_LSR(uart));
	}
}

static inline void uart_RDA_handler(void)
{
	uint8_t data;
	uint8_t ch;
	int ret;

	data = uart_port_read(UART_LSR(uart));

	while(data & UART_DATA_ABLE) {
		
		ch = uart_port_read(UART_RBR(uart)); 	
		//write_scr(&ch, 1);
		ret = uart_in_putb(ch);

		if (ret == 0) {
			ret = uart_flush_in_buff();
			uart_in_putb(ch);
		}
		
		data = uart_port_read(UART_LSR(uart));
	}
}

static inline void uart_LSC_handler(void)
{
	uint8_t data;

	data = uart_port_read(UART_LSR(uart));
	
}

static inline void uart_MSC_handler(void)
{
	uint8_t data;

	data = uart_port_read(UART_MSR(uart));
	
	if (data & UART_SEND_REQUEST) {
		
		uart_recv_start();

	} else if (data & UART_SEND_TERMINAL) {
		
		uart_recv_stop();
	
	}
}

/* uart device interrupt handler */

static void uart_irq_handler(int irq, void *arg)
{
	uint8_t data;
	
	do {
		data = uart_port_read(UART_IIR(uart)) 
			& UART_IIR_MSK;

		if (data & UART_NO_PENDING) break;

		if (data == UART_MSC_IRQ ) {
			
			uart_MSC_handler();

		} else if (data == UART_THE_IRQ) {
			
			uart_THE_handler();

		} else if (data == UART_RDA_IRQ) {
			
			uart_RDA_handler();

		} else if (data == UART_LSC_IRQ) {
			
			uart_LSC_handler();
		}
	} while (1);
		
	unmask_event(uart.irq);
}

/*
50	2,304	0x00	0x09
300	384	0x80	0x01
1,200	96	0x60	0x00
2,400	48	0x30	0x00
4,800	24	0x18	0x00
9,600	12	0x0C	0x00
19,200	6	0x06	0x00
38,400	3	0x03	0x00
57,600	2	0x02	0x00
115,200	1	0x01	0x00
*/

/* uart dddddddddevice control routine */
static int uart_dev_ioctl(unsigned long cmd)
{
	uint8_t dll, dlm, lcr;


	if (cmd & UART_BAUT_CMD) {
		lcr = uart_port_read(UART_LCR(uart));
		uart_port_write(UART_DLAB_ENABLE, UART_LCR(uart));
		switch (uart.speed) {
			case 9600: dll = 0x0C; dlm = 0x00; break;
			case 19200: dll = 0x06; dlm = 0x00; break;
			case 38400: dll = 0x03; dlm = 0x00; break;
			case 57600: dll = 0x02; dlm = 0x00; break;
			case 115200: dll = 0x01; dlm = 0x00; break;
			default: dll = uart_port_read(UART_DLL(uart));
				 dlm = uart_port_read(UART_DLM(uart));
		}	
			
		uart_port_write(dll, UART_DLL(uart));
		uart_port_write(dlm, UART_DLM(uart));
		uart_port_write(lcr, UART_LCR(uart));
	}

	return 0;
}


static int vuart_device_init(void)
{
	unsigned long serialaddr;
	int i;

	serialaddr = event_handling->serialaddr;

	if (serialaddr == 0UL) return -1;

	for(i = 0; i < XM_SERIAL_COUNT; i++) {
		rt_serial[i].in.serf = (serial_fifo_t *) serialaddr;
		rt_serial[i].in.addr =
			(char *)(serialaddr + sizeof(serial_fifo_t));

		serialaddr += PAGE_SIZE;
		rt_serial[i].out.serf = (serial_fifo_t *) serialaddr;
		rt_serial[i].out.addr =
			(char *)(serialaddr + sizeof(serial_fifo_t));

		serialaddr += PAGE_SIZE;
		rt_serial[i].in.serf->top  = 0;
		rt_serial[i].in.serf->bom  = 0;
		rt_serial[i].out.serf->top = 0;
		rt_serial[i].out.serf->bom = 0;
		rt_serial[i].in.serf->size  = PAGE_SIZE - sizeof(serial_fifo_t);
		rt_serial[i].out.serf->size = PAGE_SIZE - sizeof(serial_fifo_t);
		
		/* atomic set state flag */
		set_bit(rt_serial[i].in.serf->flag, RT_SERIAL_dom_READ);
		set_bit(rt_serial[i].in.serf->flag, RT_SERIAL_dom_WRITE);
		set_bit(rt_serial[i].out.serf->flag, RT_SERIAL_dom_READ);
		set_bit(rt_serial[i].out.serf->flag, RT_SERIAL_dom_WRITE);
	}

	uart_reset_in_buff();

	return 0;
}
/* uart device driver initialization */
static int uart_dev_init(void)
{
	int ret;

	ret = vuart_device_init();

	uart_port_read(UART_IIR(uart));
	uart_port_read(UART_LSR(uart));
	uart_port_read(UART_MSR(uart));

	uart_dev_ioctl(UART_BAUT_CMD);	
	
	uart_port_write(0x3, UART_LCR(uart));
	uart_port_write(0x8, UART_MCR(uart));
	uart_port_write(0x7, UART_FCR(uart));
	uart_port_write(0x9, UART_IER(uart));

	return 0;
}

int kmain(void)
{
	struct xmitimerval req = {{10000, 200000}, {0, 0}};

	int ret;

	ret = uart_dev_init();
	
	if(ret) {
		print("uart device initialization failed!\n");
		return -1;
	}

	set_timer (&req, 0);

	install_event_handler (uart.irq, uart_irq_handler);
	
	unmask_event (uart.irq);
	enable_events_flag();

	print("<XtratuM> VUART device loaded!\n");

	for(;;) {
		suspend_domain(0, 0);
	}

	return 0;
}

/* Read Data from serial FIFO 
 **/

int dom_serial_fifo_read(dom_fifo_t *df, uint8_t *dest, int size)
{
	serial_fifo_t *ff;
	long bom, offset, nbom;
	int rsize;

	if(df == NULL || dest == NULL || size < 0) {
		return -EINVAL;
	}

	ff = df->serf;

	if(!is_bit_set(ff->flag, RT_SERIAL_dom_READ))
		return -EPERM;

	/* There is only one domain, device domain, reads from serial fifo, 
	 * so you don't worry about concurrent problem.*/

	/* if there is fewer data left in the FIFO, read them only. */
	do {
		rsize = (ff->top + ff->size - ff->bom) % ff->size;

		if(rsize > size) rsize = size;

		bom = ff->bom;
		nbom = (bom + rsize) % ff->size;
		if (ff->top < bom && rsize > ff->size - bom) {
			offset = ff->size - bom;
			memcpy(dest, df->addr + bom, offset);
			memcpy(dest + offset, df->addr, rsize - offset);
		} else {
			memcpy((void *)dest, (void *)(df->addr + bom), rsize);

		} 
	} while (CAS(&ff->bom, bom, nbom) != bom);
	
	return rsize;
}

/* serial_fifo_write()
 *  write data to the serial device fifo
 *  only device driver domain is permitted
 *  to write data, ot it will be filled!
 * Parameters:
 *  ff: serial fifo
 *  src: specify the source memory
 *  size: data size shouled be wrote
 *
 * return:
 *  >=0: OK, writing data size
 *  < 0: failed*/

int dom_serial_fifo_write(dom_fifo_t *df, uint8_t *src, int size)
{
	serial_fifo_t *ff;
	long top, offset;
	int wsize;

	if (df == NULL || src == NULL || size < 0) 
		return -EINVAL;

	ff = df->serf;

	if (!is_bit_set(ff->flag, RT_SERIAL_dom_USED))
		return -ENODEV;

	if (!is_bit_set(ff->flag, RT_SERIAL_dom_WRITE))
		return -EPERM;

	wsize = (ff->bom + ff->size - ff->top - 1) % ff->size;

	if (wsize < size ) return -ENOMEM;

	wsize = size;

	top = ff->top;


	if ( ff->bom < top && wsize > ff->size - top) {
		offset = ff->size - top;
		memcpy(df->addr + top, src, offset);
		memcpy(df->addr, src + offset, wsize-offset);
	} else {
		memcpy(df->addr + top, src, wsize);
	}

	ff->top = (ff->top + wsize) % ff->size;

	return wsize;
}

