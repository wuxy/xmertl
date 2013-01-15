/*
 * $FILE:
 * 	rtfifo.c
 *
 * Dscription:
 * 	this is the rtfifo device
 *
 * Author:
 * 	Bai Shuwei <baishuwei@dslab.lzu.edu.cn>
 *
 * */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/gfp.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>

#include <asm/string.h>
#include <arch/mm.h>

#include <log.h>
#include <domain.h>
#include <config.h>

#ifndef __XM_KERNEL__
#define __XM_KERNEL__
#endif

#include <serial.h>
#include <xmdev.h>

static xm_serial_t xm_serials[XM_SERIAL_COUNT];

static domain_t *sdom; /* specify serial device domain */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bai Shuwei<baishw@dslab.lzu.edu.cn>");
MODULE_DESCRIPTION("Serial Device Manager!");

static int serial_irq = 4;

module_param(serial_irq, int, 0);

typedef irqreturn_t (*irq_handler_t)(int, void *, struct pt_regs *);

static irqreturn_t serial_irq_handler(int irq, void *arg, struct pt_regs *regs)
{
	return 0;
}

static int xm_request_irq(unsigned int irq, irq_handler_t handler, 
		unsigned long flag, const char *name, void *arg)
{
	return request_irq(irq, handler, flag, name, arg);

}

static void xm_free_irq(unsigned int irq, void *arg)
{
	free_irq(irq, arg);
}

static int xm_serial_mem_alloc(void)
{
	unsigned long page1;
	unsigned long page2;
	int i;
		        
	for(i = 0; i < XM_SERIAL_COUNT; i++) {
		page1 = __get_free_page(GFP_ATOMIC);
		page2 = __get_free_page(GFP_ATOMIC);
		
		if (!page1) {
			goto serial_mem_error;
		} else if (!page2) {
			free_page(page1);
			goto serial_mem_error;
		}

		memset ((unsigned char *)page1, 0, PAGE_SIZE);
		memset ((unsigned char *)page2, 0, PAGE_SIZE);

		xm_serials[i].in.serf = (serial_fifo_t *) page1;
		xm_serials[i].in.serf->addr = (char *)(page1 + sizeof(serial_fifo_t));
		xm_serials[i].in.serf->size = PAGE_SIZE - sizeof(serial_fifo_t);
		xm_serials[i].in.serf->flag = 0;
		
		xm_serials[i].out.serf = (serial_fifo_t *) page2;
		xm_serials[i].out.serf->addr = (char *)(page2 + sizeof(serial_fifo_t));
		xm_serials[i].out.serf->size = PAGE_SIZE - sizeof(serial_fifo_t);
		xm_serials[i].out.serf->flag = 0;
	}
	
	return 0;

serial_mem_error:
	for(i--; i >=0 ; i--) {
		if(xm_serials[i].in.serf) 
			free_page((unsigned long)xm_serials[i].in.serf);
		if(xm_serials[i].out.serf) 
			free_page((unsigned long)xm_serials[i].out.serf);
	}
	
	return -ENOMEM;
}

static void xm_serial_mem_free(void)
{
	int i;
	
	for(i = 0; i < XM_SERIAL_COUNT; i++) {
		
		if(xm_serials[i].in.serf) free_page((unsigned long)xm_serials[i].in.serf);
		if(xm_serials[i].out.serf) free_page((unsigned long)xm_serials[i].out.serf);
	}
}

static unsigned long xm_map_page(unsigned long pd, 
	unsigned long addr, unsigned long vaddr, unsigned long flag)
{
	unsigned long pd_entry = va2pd(vaddr);
	unsigned long pt_entry = va2pt(vaddr);
	unsigned long nflag;
	unsigned long pt;

	if( !((((unsigned long *)pd)[pd_entry]) & _PAGE_PRESENT) ) {
		pt = __get_free_page(GFP_ATOMIC);
		if( !pt ) return -ENOMEM;
		
		memset((unsigned char *)pt, 0, PAGE_SIZE);
		
		nflag = flag & (~_PAGE_UNUSED1);
		fill_pd_entry (pd, pd_entry, pt, nflag);
	} else {
		pt = get_pd_addr(pd, pd_entry);
	}

	nflag = flag;
	if ( !(((unsigned long *)pt)[pt_entry] & _PAGE_PRESENT)) {
		fill_pt_entry (pt, pt_entry, addr, nflag);
	}

	return 0;
}

static int xm_unmap_page(unsigned long pd,
		        unsigned long vaddr)
{
	unsigned long pd_entry = va2pd(vaddr);
	unsigned long pt_entry = va2pt(vaddr);
	unsigned long pt;

	if (((unsigned long *)pd)[pd_entry] & _PAGE_PRESENT) {
		pt = get_pd_addr(pd, pd_entry);
		if(((unsigned long *)pt)[pt_entry] & _PAGE_PRESENT) {
			((unsigned long *)pt)[pt_entry] = 0UL;
		}
	}

	return 0;
}

/* xm_serial_xm_map()
 *   called by xtratum to map the domain 
 *   space to fifo physical address 
 * Paramenters:
 *   paddr: physical address 
 *   vaddr: virtual address
 *   size:  memory size
 *   flag:  memory page flag*/

int xm_serial_xm_map(unsigned long pd, 
	unsigned long vaddr, unsigned long size, 
	void *arg)
{
	unsigned long nflag = _PAGE_UNUSED1 | 
		_PAGE_PRESENT | _PAGE_RW | _PAGE_USER;
	unsigned long nvaddr = DEFAULT_SERIAL_ADDR;
	unsigned long addr;
	unsigned long i = (unsigned long)arg;
	int ret;

	/* the domain is normal domain, not a serial device */
	if (size == 0) {
		if(i >= XM_SERIAL_COUNT) {
			return -ENODEV;
		} 
		
		if( is_bit_set(xm_serials[i].in.serf->flag, RT_SERIAL_dom_INIT) || 
			is_bit_set(xm_serials[i].out.serf->flag, RT_SERIAL_dom_INIT)) 
		{
			XMLOG("serial device has been used\n");
			return -EBUSY;
		}
		addr = (unsigned long)xm_serials[i].in.serf;
		ret = xm_map_page(pd, addr, nvaddr, nflag);
		
		if(ret) {
			XMLOG("[a] xm_map_page failed\n");
			return ret;
		}

		addr = (unsigned long)xm_serials[i].out.serf;
		ret = xm_map_page(pd, addr, nvaddr+PAGE_SIZE, nflag);
		if(ret) {
			XMLOG("[b] xm_map_page failed\n");
			return ret;
		}

		set_bit(xm_serials[i].in.serf->flag, RT_SERIAL_dom_INIT);
		set_bit(xm_serials[i].out.serf->flag, RT_SERIAL_dom_INIT);

	} else {
		/* serial device driver */
		for (i = 0; i < XM_SERIAL_COUNT; i++) {
			addr = (unsigned long)xm_serials[i].in.serf;
			ret = xm_map_page(pd, addr, nvaddr, nflag);
			if(ret) {
				XMLOG("[c] xm_map_page failed\n");
				return ret;
			}
		
			addr = (unsigned long)xm_serials[i].out.serf;
			ret = xm_map_page(pd, addr, nvaddr+PAGE_SIZE, nflag);
			if(ret) {
				XMLOG("[d] xm_map_page failed\n");
				return ret;
			}
			
			nvaddr = nvaddr + 2*PAGE_SIZE;
			
			set_bit(xm_serials[i].in.serf->flag, RT_SERIAL_dev_INIT);
			set_bit(xm_serials[i].out.serf->flag, RT_SERIAL_dev_INIT);
		}
	}
	
	return 0;
}

/* xm_serial_xm_unmap()
 *   called by xtratum to unmap the
 *   domain memory address */

int xm_serial_xm_unmap(unsigned long pd,
	unsigned long vaddr, unsigned long size)
{
	unsigned long pd_entry;
	unsigned long nvaddr;
	unsigned long pt;
	int i;

	/* address should be aligned with page size */
	if (vaddr & (PAGE_SIZE -1)) {
		return -EINVAL;
	}

	nvaddr = DEFAULT_SERIAL_ADDR;

	/* non-serial driver domain */
	if (size == 0) {
		xm_unmap_page(pd, nvaddr);
		xm_unmap_page(pd, nvaddr+PAGE_SIZE);
	} else {
	/* serial driver domain */
		for (i = 0; i < XM_SERIAL_COUNT; i++) {
			xm_unmap_page(pd, nvaddr);
			xm_unmap_page(pd, nvaddr+PAGE_SIZE);
			nvaddr = nvaddr + 2*PAGE_SIZE;
			
			clear_bit(xm_serials[i].in.serf->flag, RT_SERIAL_dev_INIT);
			clear_bit(xm_serials[i].out.serf->flag, RT_SERIAL_dev_INIT);
		}
	}
	
	pd_entry = va2pd(vaddr);
	pt = get_pd_addr(pd, pd_entry);

	for ( i = 0; i < 1024; i++ ) {
		if(((unsigned long *)pt)[i]) {
			break;
		}
	}

	if (i == 1024) {
		((unsigned long *)pd)[pd_entry] = 0UL;
		free_page(pt);
	}

	return 0;	
}

static int xm_serial_xm_ioctl(unsigned long minor,
		unsigned long cmd, void *arg)
{
	domain_t *dom;
	int ret = 0;

	if (minor > XM_SERIAL_COUNT) {
		return -ENODEV;
	}

	if (minor == XM_SERIAL_COUNT) {
	/* serial device driver */
		if (cmd & XMDEV_IO_INIT) {
			if (sdom == NULL) sdom = (domain_t*)arg;
			else if (sdom != (domain_t *)arg) {
				return -EBADF;
			}
		} 
		return 0;
	}

	/* NON serial device driver */
	if(cmd & XMDEV_IO_CLOSE) {
		clear_bit(xm_serials[minor].flag, XM_SERIAL_DEV_USED);
		clear_bit(xm_serials[minor].in.serf->flag, RT_SERIAL_dev_INIT);
		clear_bit(xm_serials[minor].out.serf->flag, RT_SERIAL_dev_INIT);
		xm_serials[minor].dom = NULL;
	}

	/* special command to initialize the serail domain */
	if(cmd & XMDEV_IO_OPEN) {
		dom = (domain_t *)arg;
		if ((xm_serials[minor].flag & ( 1 << XM_SERIAL_DEV_USED)) && 
				(xm_serials[minor].dom != dom) ) {
			return -EBUSY;
		}

		set_bit(xm_serials[minor].flag, XM_SERIAL_DEV_USED);
		xm_serials[minor].dom = dom;

		ret = xm_serial_xm_map(dom->pd,
				DEFAULT_SERIAL_ADDR, 0, (void *)minor);
		if( ! ret ) {
			if( dom->events) {
				dom->events->serialaddr = DEFAULT_SERIAL_ADDR;
			} else {
				XMBUG();
			}
		} else {
			clear_bit(xm_serials[minor].flag, XM_SERIAL_DEV_USED);
			xm_serials[minor].dom = NULL;
		}
	} else if(cmd & XMDEV_IO_WKUP) {
		/* wake up the serial device domain */
		if(sdom != NULL) set_domain_state(sdom, DOMAIN_ACTIVE);
	}

	return ret;
}

static xm_dev_t 
xm_serial_dev = {
	XM_SERIAL_DEV,
	xm_serial_xm_map,
	xm_serial_xm_unmap,
	xm_serial_xm_ioctl,
	0
};

/* xm_serial_xm_init()
 *  register the rt serial device */

struct resource *serial_res;

int xm_serial_xm_init(void)
{
	int ret, irq;


	serial_res = request_region(SERIAL_BASE_ADDR, SERIAL_PORT_SIZE, "serial");
/*	
	if (serial_res == NULL) {
		return -1;
	}	
*/	
	for (ret = 0; ret < XM_SERIAL_COUNT; ret++) {
		xm_serials[ret].flag = 0;
		xm_serials[ret].dom = NULL;
	}

	/* allocate physical memory for serial device */
	ret = xm_serial_mem_alloc();

	if ( ret ) return ret;

	ret = xm_dev_register(XM_SERIAL_DEV, &xm_serial_dev);

	if ( ret ) return ret;

	irq = xm_request_irq(serial_irq, serial_irq_handler, SA_INTERRUPT, "serial", NULL);
	
	if(irq < 0) {
		return -1;
	}

	XMLOG("XM Serial Dev(vttyS0~vttyS15)!\n");

	return 0;
}

void xm_serial_xm_exit(void)
{
	int ret, i;
	
	xm_free_irq(serial_irq, NULL);

	ret = xm_dev_unregister(XM_SERIAL_DEV);

	if (ret != 0) {
		XMLOG("Serial Device is USED!\n"); 
		return; 
	}

	xm_serial_mem_free();

	for (i = 0; i < XM_SERIAL_COUNT; i++) {
		xm_serials[i].flag = 0;
		xm_serials[i].dom = NULL;
	}

	release_region(SERIAL_BASE_ADDR, SERIAL_PORT_SIZE);

	XMLOG("XM Serial Dev Exit!\n");
}

/* Two routines are called by linux only!!! */


int rt_console_get(int minor, 
	unsigned long *in, unsigned long *out)
{
	if (minor >= XM_SERIAL_COUNT) {
		return -ENODEV;
	}

	if ((xm_serials[minor].flag & ( 1 << XM_SERIAL_DEV_USED))) {
		return -EBUSY;
	}

	set_bit(xm_serials[minor].flag, XM_SERIAL_DEV_USED);

	*in = (unsigned long)xm_serials[minor].in.serf;
	*out = (unsigned long)xm_serials[minor].out.serf;

	set_bit(xm_serials[minor].in.serf->flag, RT_SERIAL_dom_INIT);
	set_bit(xm_serials[minor].out.serf->flag, RT_SERIAL_dom_INIT);
	
	return 0;
}

int rt_console_free(int minor)
{
	if (minor >= XM_SERIAL_COUNT) 
		return -ENODEV;
	
	xm_serials[minor].in.serf->flag = 0;
	xm_serials[minor].out.serf->flag = 0;
	xm_serials[minor].flag = 0;

	return 0;
}

EXPORT_SYMBOL(rt_console_get);
EXPORT_SYMBOL(rt_console_free);

module_init(xm_serial_xm_init);
module_exit(xm_serial_xm_exit);
