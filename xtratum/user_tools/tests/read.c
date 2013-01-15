#include <xm_syscalls.h>
#include <xm/xmdev.h>
#include <xm/serial.h>
#include <doms/stdlib.h>
#include <doms/string.h>
#include <doms/errno.h>
//#include <doms/bits.h>
#include <devbug.h>
#include <io.h>
#include "../serial/uart.h"

#define Console_MINOR 0x5
#define Console_MAJOR XM_SERIAL_DEV

static uart_dev_t uart = {4, 0x3f8, 0, 9600};
static unsigned int my_Console_ttyS = 5;

#define XM_2_DEV(minor, major) \
	(minor | ((major << 16) & 0xffff0000))

static rt_serial_t rt_serial;

int dom_serial_fifo_write(dom_fifo_t *df, const char *src, int size)
{
	serial_fifo_t *ff;
        long top, offset;
        int wsize;

        if (df == NULL || src == NULL || size < 0)
                return -EINVAL;

	ff = df->serf;

        if (!is_bit_set(ff->flag, RT_SERIAL_dom_WRITE))
                return -EPERM;

        wsize = (ff->bom + ff->size - ff->top - 1) % ff->size;

	
        if (wsize < size) return -ENOMEM;

	wsize = size;

    	top = ff->top;

        if ( (ff->bom <= top) && (ff->size - top < wsize)) {
		offset = ff->size - top;
		memcpy(df->addr + top, src, offset);
		memcpy(df->addr, src+offset, wsize-offset);
	} else {
		memcpy(df->addr + top, src, wsize);
        }

	ff->top = (ff->top + wsize) % ff->size;
        
	return wsize;
}

int dom_serial_fifo_read(dom_fifo_t *df, char *dest, int size)
{
	serial_fifo_t *ff;
	long bom, offset, nbom;
	int rsize;

        
	if(df == NULL || dest == NULL || size < 0) 
		return -EINVAL;
	
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
		if ((ff->top < bom) && (rsize > ff->size - ff->bom)) {
			offset = ff->size - bom;
			memcpy(dest, df->addr + bom, offset);
			memcpy(dest + offset, df->addr, rsize - offset);
		} else {
			memcpy(dest, df->addr + bom, rsize);
		}

	} while (CAS(&ff->bom, bom, nbom) != bom);									        
											        
	return rsize;
}

int rt_serial_read(char *dst, int size)
{
	int err;

	err = dom_serial_fifo_read(&rt_serial.in, dst, size);		
	
	return err;
}

int rt_serial_write(const char *src, int size)
{
	int ret;
	
	ret = dom_serial_fifo_write(&rt_serial.out, src, size);

	/* enable THRE interrupt */
	if(ret > 0) {
		
		outb(0xB, UART_IER(uart));		
		outb(0x9, UART_MCR(uart));
	}

	return ret;
}

static int serial_device_init(unsigned int ttys, long baud)
{
	unsigned long serialaddr;
	unsigned long devid;
	serial_fifo_t *serf;

	/* hardware serial device number */
	if( ttys >= 16 ) {
		return -ENODEV;
	}

	if( baud < 0) 
		return -EINVAL;

	devid = XM_2_DEV(Console_MINOR, Console_MAJOR);
	
	suspend_domain(devid, XMDEV_IO_OPEN);
	serialaddr = event_handling->serialaddr;
	serf = (serial_fifo_t *)serialaddr;
	
	/* register serial device */
	if( !is_bit_set(serf->flag, RT_SERIAL_dev_INIT) )
	       return -EPIPE;

	if( is_bit_set(serf->flag, RT_SERIAL_dom_USED) )
		return -EBUSY;	
	
	set_bit(serf->flag, RT_SERIAL_dom_USED);
	
	/* register serial device */
	
	rt_serial.in.serf = (serial_fifo_t *)serialaddr;
	rt_serial.in.addr = (char *)(serialaddr + sizeof(serial_fifo_t));
	serialaddr += PAGE_SIZE;
	rt_serial.out.serf = (serial_fifo_t *)serialaddr;
	rt_serial.out.addr = (char *)(serialaddr + sizeof(serial_fifo_t));

	serf->baud = baud;
	serf->prio = Console_MINOR;
	serf->flag |= (ttys << 24) & RT_SERIAL_dom_DEST_MASK; /* Destination Console Device */
	
	serf = rt_serial.out.serf;
	
	set_bit(serf->flag, RT_SERIAL_dom_USED);
	
	serf->baud = baud;
	serf->prio = Console_MINOR;
	serf->flag |= (ttys << 24) & RT_SERIAL_dom_DEST_MASK; /* Destination Console Device */

	return 0;
}

void timer_handler(int irq, void *arg)
{
	unmask_event(0);
}

static char string[4096];

int kmain (void) 
{
	struct xmitimerval req = {{0, 200000}, {0, 0}};
	struct xmtimespec st, et;
	long tm;
	long baud = 156000;
	int ret;
	int size = 0;


	ret = serial_device_init(my_Console_ttyS, baud);

	set_timer(&req, 0);
	
	install_event_handler(0, timer_handler);

	unmask_event(0);
	enable_events_flag();
	
	//rt_serial_write(string, 30);
	get_time(&st);
	while (1) { 
		//suspend_domain (0, 0);

		ret = rt_serial_read(string, 1024);
		if(ret > 0) {
			size += ret;
		}

		if(size > 1024*1024) {
			get_time(&et);
			break;
		}
	}

    tm = (et.tv_sec-st.tv_sec)*1000000+et.tv_nsec-st.tv_nsec;

	hexprint(tm);
	
	return 0;
}
