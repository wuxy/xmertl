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

#include <asm/string.h>
#include <linux/errno.h>

#include <arch/mm.h>
#include <log.h>
#include <xmdev.h>
#include "rtfifo.h"

static unsigned long fpages[XM_FIFO_COUNT] = {0, }; // specify the fifo data page
static unsigned long fpage = 0;              // specify the fifo control page

static struct fifo_struct *fifos[XM_FIFO_COUNT];   // specify the fifo control structure

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bai Shuwei<baishw@dslab.lzu.edu.cn>");
MODULE_DESCRIPTION("RT FIFO Management Module");

#define vaddr_to_paddr(vaddr) (vaddr & 0x3FFFFF)

static int alloc_fifo_data_page(void)
{
	int i;
		        
	for(i = 0; i < XM_FIFO_COUNT; i++) {
		fpages[i] = __get_free_page(GFP_ATOMIC);
		
		if(fpages[i]) {
			memset ((unsigned char *)fpages[i], 0, PAGE_SIZE);
		} else {
			goto fifo_data_page_erro;
		}
	}
	
	return 0;

fifo_data_page_erro:
	for(i = 0; i < XM_FIFO_COUNT; i++) {
		if(fpages[i]) free_page(fpages[i]);
	}
	
	return -ENOMEM;
}

static void free_fifo_data_page(void)
{
	int i;

	for(i = 0; i < XM_FIFO_COUNT; i++) {
		if(fpages[i]) free_page(fpages[i]);
	}
}

static int fifo_control_init(void)
{
	int i;

	if(!(fpage = __get_free_page(GFP_ATOMIC))) {
		return -ENOMEM;
	}

	memset ((unsigned char *)fpage, 0, PAGE_SIZE);

	for(i = 0; i < XM_FIFO_COUNT; i++) {
		fifos[i] = (struct fifo_struct *) 
			(fpage + i * sizeof(struct fifo_struct));
		FIFO_TOP(fifos[i]) = 0;
		FIFO_BOM(fifos[i]) = 0;
		FIFO_SIZE(fifos[i]) = PAGE_SIZE;
		FIFO_ADDR(fifos[i]) = (char *)fpages[i];
		FIFO_FLAG(fifos[i]) |= FIFO_US;
	}

	return 0;
}

static void fifo_control_exit(void)
{
	if(fpage) free_page(fpage);
}

static int rt_fifo_mem_alloc(void)
{
        if(alloc_fifo_data_page()) {
		goto data_fail;
	}

	if(fifo_control_init()) {
		goto ctl_fail;
        }

	return 0;

ctl_fail:
	free_fifo_data_page();
data_fail:
	return -ENOMEM;
}

static int xm_map_page(unsigned long pd, 
	unsigned long addr, unsigned long vaddr, unsigned long flag)
{
	unsigned long pd_entry = va2pd(vaddr);
	unsigned long pt_entry = va2pt(vaddr);
	unsigned long nflag;
	unsigned long pt;


	if( !((((unsigned long *)pd)[pd_entry]) & _PAGE_PRESENT) ) {
		pt = __get_free_page(GFP_ATOMIC);
		if( !pt ) return 0;
		memset((unsigned char *)pt, 0, PAGE_SIZE);
		nflag = flag & (~_PAGE_UNUSED1);
		fill_pd_entry(pd, pd_entry, pt, nflag);
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

	if (((unsigned long *)pd)[pd_entry] & _PAGE_PRESENT ) {
		pt = get_pd_addr(pd, pd_entry);
		if(((unsigned long *)pt)[pt_entry] & _PAGE_PRESENT) {
			((unsigned long *)pt)[pt_entry] = 0UL;
		}
	}

	return 0;
}

/* rt_fifo_xm_map()
 *   called by xtratum to map the domain 
 *   space to fifo physical address 
 * Paramenters:
 *   paddr: physical address 
 *   vaddr: virtual address
 *   size:  memory size
 *   flag:  memory page flag*/


int rt_fifo_xm_map(unsigned long pd, 
	unsigned long vaddr, unsigned long size,
	void *arg)
{
	unsigned long nflag = _PAGE_UNUSED1 | 
		_PAGE_PRESENT | _PAGE_RW | _PAGE_USER;
	unsigned long nvaddr = vaddr;
	int i;

	/* address should be aligned with page size */
	if (vaddr & (PAGE_SIZE -1)) {
		return -EINVAL;
	}
	
	/* paddr = vaddr_to_paddr(fpage); */

	xm_map_page(pd, fpage, nvaddr, nflag);

	for ( i = 0; i < XM_FIFO_COUNT; i++) {
		//paddr = vaddr_to_paddr(fpages[i]);
		nvaddr = nvaddr + PAGE_SIZE;	
		xm_map_page(pd, fpages[i], nvaddr, nflag);
	}
	
	return 0;
}

/* rt_fifo_xm_unmap()
 *   called by xtratum to unmap the
 *   domain memory address */

int rt_fifo_xm_unmap(unsigned long pd,
	unsigned long vaddr, unsigned long size)
{
	unsigned long pd_entry;
	unsigned long nvaddr = DEFAULT_FIFO_ADDR;
	unsigned long pt;
	int i;

	/* address should be aligned with page size */

	xm_unmap_page(pd, nvaddr);
	for (i = 0; i < XM_FIFO_COUNT; i++) {
		nvaddr += PAGE_SIZE;
		xm_unmap_page(pd, nvaddr);
	}
	
	pd_entry = va2pd(DEFAULT_FIFO_ADDR);
	pt = get_pd_addr(pd, pd_entry);

	for (i = 0; i < 1024; i++) {
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

static int rt_fifo_xm_ioctl(unsigned long minor,
		unsigned long cmd, void *arg)
{
	if(minor > XM_FIFO_COUNT) {
		return -EINVAL;
	}
	
	return 0;
}

static xm_dev_t 
xm_rtf_dev = {
	XM_FIFO_DEV,
	rt_fifo_xm_map,
	rt_fifo_xm_unmap,
	rt_fifo_xm_ioctl,
	0
};

/* rt_fifo_xm_init()
 *  register the rtfifo device */

int rt_fifo_xm_init(void)
{
	int ret;
	/* allocate physical memory for fifo device */
	ret = rt_fifo_mem_alloc();

	if ( ret ) return ret;

	ret = xm_dev_register(XM_FIFO_DEV, &xm_rtf_dev);
	
	if ( ret ) return ret;

	XMLOG("XM FIFO Dev(rtf0~rtf15)!\n");

	return 0;
}

void rt_fifo_xm_exit(void)
{
	int ret;

	ret = xm_dev_unregister(XM_FIFO_DEV);

	if (ret != 0) {
		XMLOG("XM FIFO Device is USED!\n");
		return;
	}

	fifo_control_exit();
	free_fifo_data_page();
	
	XMLOG("XM FIFO Dev Exit!\n");

}

/* rt_fifo_user** routines are used for root domain
 * In the system the root domain is linux *********/

int rt_fifo_user_read(int index, char *dest, int size)
{
	int sz = size;
	long old_bom;
	long new_bom;
	long bom;
	struct fifo_struct *ff;
	
	if(INDEX_UNVALID(index) || ADDR_UNVALID(dest) || sz < 0) {
		return -EINVAL;
	}

	ff = fifos[index];

        if(!FIFO_USABLE(ff)) {
                return -ENODEV;
        }

/*
        if(FIFO_WRITING(ff)) {
                ff = fifos[(index +XM_FIFO_COUNT) % XM_FIFO_COUNT*2]; 
        }
*/
	SET_READ_FLAGS(ff);

	do {
		
	        if(sz > FIFO_USED_SIZE(ff)) {
			sz = FIFO_USED_SIZE(ff);
        	}

		old_bom = FIFO_BOM(ff);
		new_bom = old_bom + sz;
		bom = old_bom % FIFO_SIZE(ff);
		
		if(FIFO_TOP(ff) % FIFO_SIZE(ff) >= bom) {
			memcpy(dest, FIFO_ADDR(ff) + bom, sz);
		} else {
			if(sz >  FIFO_SIZE(ff) - bom) {
				memcpy(dest, FIFO_ADDR(ff) + bom, FIFO_SIZE(ff) - bom); 
				memcpy(dest + FIFO_SIZE(ff) -  bom, FIFO_ADDR(ff), 
					sz - (FIFO_SIZE(ff) - bom));
			} else {
				memcpy(dest, FIFO_ADDR(ff) + bom, sz);
			}	
		}

	} while(CAS(&FIFO_BOM(ff), old_bom, new_bom) != old_bom);

	UNSET_READ_FLAGS(ff);

	if(FIFO_TOP(ff) == FIFO_BOM(ff)) {
		FIFO_TOP(ff) = 0;
		FIFO_BOM(ff) = 0;
	}
	return sz;	
}

int rt_fifo_user_write(int index, const char *src, int size)
{
	int sz = size;
	unsigned long old_top;
	unsigned long new_top;
	unsigned long top;
	struct fifo_struct *ff;

        if(INDEX_UNVALID(index) || ADDR_UNVALID(src) || sz < 0) {
                return -EINVAL;
        }
	ff = fifos[index];

        if(!FIFO_USABLE(ff)) {
                return -ENODEV;
        }

/*
	if(FIFO_READING(ff)) {
		ff = fifos[(index + XM_FIFO_COUNT) % (XM_FIFO_COUNT * 2)]; 
	}
*/	
	SET_WRITE_FLAGS(ff);

	do {
	        if(sz > FIFO_FREE_SIZE(ff)) {
			sz = FIFO_FREE_SIZE(ff);
                	//return 0;
		}

		old_top = FIFO_TOP(ff);
		new_top = FIFO_TOP(ff) + sz;
		top = FIFO_TOP(ff) % FIFO_SIZE(ff);

		if(top <= FIFO_BOM(ff) % FIFO_SIZE(ff)) {
			memcpy(FIFO_ADDR(ff) + top, src, sz);
		} else {
			if(FIFO_SIZE(ff) - top >= sz) {
				memcpy(FIFO_ADDR(ff) + top, src, sz);
			} else {
				memcpy(FIFO_ADDR(ff) + top, src, \
					FIFO_SIZE(ff) - top);
				memcpy(FIFO_ADDR(ff), src + FIFO_SIZE(ff) - top, \
					sz - (FIFO_SIZE(ff) - top));
			}
		}
	} while(CAS(&FIFO_TOP(ff), old_top, new_top) != old_top);

	UNSET_WRITE_FLAGS(ff);

	return sz;
}

static int rt_fifo_user_flush(int index)
{
	struct fifo_struct *ff;

	if(INDEX_UNVALID(index)) {
                return -EINVAL;
        }
	
	ff = fifos[index];

	FIFO_TOP(ff) = 0;
	FIFO_BOM(ff) = 0;

	return 0;
}

int rt_fifo_user_ioctl(int index, int cmd, void *accept)
{
	int ret;

	if(INDEX_UNVALID(index)) {
                return -EINVAL;
        }

	switch(cmd) {
		case FLUSH_FIFO: 
			if((ret = rt_fifo_user_flush(index)) != 0) {
				return ret;
			} else return 0;
		default: return 0;				
	}

	return 0;
}


EXPORT_SYMBOL(rt_fifo_user_read);
EXPORT_SYMBOL(rt_fifo_user_write);
EXPORT_SYMBOL(rt_fifo_user_ioctl);

module_init(rt_fifo_xm_init);
module_exit(rt_fifo_xm_exit);
