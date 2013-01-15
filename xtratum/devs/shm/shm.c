/*
 * $FILE: shm.c 
 *
 * $Description:
 * 	
 *
 * $VERSION: 1.0
 *
 * Author: Bai Shuwei <baishuwei@{dslab.lzu.edu.cn, weslab.com}>
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
 */


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/gfp.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

#include <asm/string.h>
#include <linux/errno.h>

#include <arch/mm.h>
#include <arch/kmem.h>

#include <domain.h>
#include <tlsf.h>
#include <log.h>
#include <xmdev.h>

#include "shm.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bai Shuwei<baishuwei@weslab.com>");
MODULE_DESCRIPTION("SHM DEIVCES MANAGEMENT MODULE");

static xm_shm_t xshm_list[XM_SHM_COUNT];

static void * xshm_pool = NULL;

static int xm_shm_name_check(const char *name)
{
	int i;
	
	for (i = 0; name[i] != '\0'; i++);

	if (i == 0 || i >= SHM_NAME_SIZE)
		return -EINVAL;

	return 0;
}

static int xm_strcmp(const char *str1, const char *str2) 
{
	while((*str1) == (*str2)) {
		if((*str1) == '\0') break;
		str1 ++;
		str2 ++;
	}

	/* not */
	if((*str1) == (*str2)) return 0;
	else return -1;
}

static int xm_get_shm_by_name(const char *name)
{
	int index = 0;
	
	if(xm_shm_name_check(name)) return -EINVAL;

	for (index = 0; index < XM_SHM_COUNT; index ++) {
		if (xm_strcmp(xshm_list[index].name, name) == 0) {
			break;
		}
	}

	return index;
}

/*
 * routine: xm_shm_create
 *
 * parameters: 
 *   name: specify the new shm device name
 *   size: specify the size of new shm device
 *
 * return value:
 *   failure   : XM_SHM_COUNT
 *   successful: [0, XM_SHM_COUNT)
 *
 * */
static int xm_shm_create(const char *name)
{
	unsigned long size = SHM_MEM_SIZE;

	int index, i;

	if(xm_shm_name_check(name)) return -EINVAL;
	
	index = xm_get_shm_by_name(name);

	if (index == XM_SHM_COUNT) {
		for(index = 0; index < XM_SHM_COUNT; index++) {
			if(xshm_list[index].owner == NULL) {

				xshm_list[index].owner = xm_current_domain;
			
				/* be careful with alignment*/
				xshm_list[index].va_start = ((unsigned long) malloc_ex(size, xshm_pool)  + 0xFFF) & 0xFFFFF000;

				if( !xshm_list[index].va_start) {
					xshm_list[index].owner = NULL;					

					return -ENOMEM;
				}
			
				for(i = 0; (i < SHM_NAME_SIZE-1) && (name[i] != '\0'); i++) {
					xshm_list[index].name[i] = name[i];
				}
				xshm_list[index].name[i] = '\0';

				xshm_list[index].size = size;

				break;
			}
		}
	
	}

	if(index < XM_SHM_COUNT) xshm_list[index].users ++;

	return index;
}

int xm_shm_open(const char *name, 
	unsigned long flag, unsigned long mode)
{
	int index = 0;
	if(xm_shm_name_check(name)) return -EINVAL;

	index = xm_shm_create(name);

	if(index == XM_SHM_COUNT) return -ENOMEM;

	xshm_list[index].users ++;

	return index;
}

EXPORT_SYMBOL(xm_shm_open);

/*
 * routine: xm_shm_release
 *
 * parameter:
 * 	 index: the shm device index in the shm list
 *
 * return value:
 *   failure:Not 0
 *   successful: 0
 *
 * */

static int xm_shm_release(const int index)
{
	xshm_list[index].name[0] = '\0';

	free_ex((void *)xshm_list[index].va_start, xshm_pool);

	xshm_list[index].owner = NULL;
	xshm_list[index].users --;

	return 0;
}

int xm_shm_unlink(int index)
{
	if (index >= XM_SHM_COUNT)
		return -ENODEV;


	if (xshm_list[index].owner != xm_current_domain) {
		xshm_list[index].users --;
	} else {
		if (xshm_list[index].users != 1)
			return -EBUSY;
		xm_shm_release(index);
	}

	return 0;
}

EXPORT_SYMBOL(xm_shm_unlink);

/*
 * routine:
 *   xm_shm_map
 *
 * parameters:
 *   pd: the highest level page directory, physical address
 *   vaddr: virtual address expected in the domain address
 *   size:  map memory size expected by domain
 *   arg:   the linux kernel virtual address
 *
 * */

static int xm_shm_map(unsigned long pd,
	unsigned long vaddr, unsigned long size,
	void *arg)
{
	unsigned long nflag = _PAGE_UNUSED1 |
			 _PAGE_PRESENT | _PAGE_RW | _PAGE_USER;
	unsigned long nvaddr = vaddr & (~(PAGE_SIZE - 1));
	unsigned long nsize = size + (vaddr & (PAGE_SIZE - 1));
	unsigned long kvaddr = (unsigned long) arg;
	unsigned long kladdr = 0UL;
	int ret = 0;

	kvaddr = kvaddr & PAGE_MASK;
	while ( nsize >= PAGE_SIZE ) {
		kladdr = kmem_kvaddr_to_kladdr(kvaddr);
		
		kmem_page_map(pd, nflag, nvaddr, kladdr);
		
		nvaddr = nvaddr + PAGE_SIZE;
		kvaddr  = kvaddr  + PAGE_SIZE;
		nsize  = nsize - PAGE_SIZE;
	} 

	if (nsize > 0) {
		kladdr = kmem_kvaddr_to_kladdr(kvaddr);

		if ( kmem_page_map(pd, nflag, nvaddr, kladdr) == nvaddr)
			ret = 0;
	}
	return ret;
}


/*
 * routine: xm_shm_unmap
 *
 * parameters:
 *   pd:
 *   vaddr:
 *   size:
 *
 * return value:
 *   error:
 *   successful:
 *
 * */
static int xm_shm_unmap(unsigned long pd,
	unsigned long dvaddr, unsigned long size)
{
	unsigned long nvaddr = dvaddr & (~(PAGE_SIZE - 1));
	unsigned long nsize = size + (dvaddr & (PAGE_SIZE - 1));
	unsigned long pd_entry, pmtab;

	int i;

	while( nsize >= PAGE_SIZE ) {
		kmem_page_unmap(pd, nvaddr);

		nvaddr = nvaddr + PAGE_SIZE;
		nsize  = nsize  - PAGE_SIZE;
	}

	if( nsize > 0 ) {
		kmem_page_unmap(pd, nvaddr);
	}


	pd_entry = va2pd(dvaddr);
    pmtab = get_pm_addr(pd, pd_entry);

	for (i = 0; i < 1024; i++) {
		if(((unsigned long *)pmtab)[i]) {
			break;
		}
	}

	if (i == 1024) {
		((unsigned long *)pd)[pd_entry] = 0UL;
		free_page(pmtab);
	}

	return 0;
}

static int xm_shm_resize(unsigned long index, unsigned long size)
{
	if(xshm_list[index].size < size) return -EINVAL;

	xshm_list[index].size = size;

	return 0;
}

/*
 * routine: xm_shm_ioctl
 *
 * description:
 *   It used by XM SHM DEV, and Linux SHM Device 
 * parameters:
 *   minor: the device minor number, which is the list index too
 *   cmd:   specify the device operation
 *   arg:   different meaning follows cmd value
 *   
 * */

int xm_shm_ioctl(unsigned long minor,
	unsigned long cmd, void *arg)
{
	xm_shm_t *shm = NULL;
	int ret = 0;

	if (minor > XM_SHM_COUNT) return -ENODEV;

	shm = &xshm_list[minor];

	
	if (minor == XM_SHM_COUNT) {
		if (XM_SHM_CREATE == cmd) {
			/*
		 	 * create the SHM device with name
		 	 * arg: specify the device name
		 	 * */

		 	ret = xm_shm_create((const char *)arg);
		} else if (XM_SHM_EXIST == cmd) {
			/*
			 * check whether the device has been created
			 * arg: specify the device name
			 * */
			ret = xm_get_shm_by_name((const char *)arg);
		}
	} else {
		if (XM_SHM_REMOVE == cmd) {
		/* remove or unlink the device with name */
			ret = xm_shm_unlink(minor);
			
		} else if (XM_SHM_TRUNCATE == cmd ) {
		/* truncate the SHM size 
		 * arg: specify the new shm size 
		 * */
			ret = xm_shm_resize(minor, (unsigned long)arg);
		
		} else if (XM_SHM_MAP == cmd) {

		/* map the SHM to the expect space address, 
		 * arg specify the domain virtual address */
			ret = xm_shm_map(xm_current_domain->pd, (unsigned long)arg, shm->size, (void *)shm->va_start);
		
		} else if (XM_SHM_UNMAP == cmd) {

		/* unmap the SHM memory 
		 * arg specify the domain virtual address */
			ret = xm_shm_unmap(xm_current_domain->pd, (unsigned long)arg, shm->size);	
		} else if (XM_SHM_SIZE == cmd) {
			*(unsigned long *)arg = xshm_list[minor].size;
		
		} else if (XM_SHM_VADDR == cmd) {
			*(unsigned long *)arg = xshm_list[minor].va_start;
		}
	}

	return ret;
}

EXPORT_SYMBOL(xm_shm_ioctl);

static xm_dev_t
xm_shm_dev = {
	XM_SHM_DEV,
	xm_shm_map,
	xm_shm_unmap,
	xm_shm_ioctl,
	0
};


static int xm_shmpool_init(void)
{
	xshm_pool = vmalloc(SHM_TOTAL_SIZE);

	if( xshm_pool == NULL) {
		return -ENOMEM;
	}

	if(init_memory_pool(SHM_TOTAL_SIZE, xshm_pool) < 0) {
		return -1;
	}

	return 0;
}

static int xm_shmpool_destroy(void)
{
	int i;

	for (i = 0; i < XM_SHM_COUNT; i++) {
		if(xshm_list[i].users) {
			XMLOG("SHM device is busy!\n");

			return -EBUSY;
		}
	}

	return 0;
}



int shm_xm_init(void)
{
	int ret = 0;

	ret = xm_shmpool_init();

	if(ret) return ret;

	for (ret = 0; ret < XM_SHM_COUNT; ret ++)
		xshm_list[ret].owner = NULL;

	ret = xm_dev_register(XM_SHM_DEV, &xm_shm_dev); 

	if(ret) return ret;
	
	XMLOG("XtratuM SHM devices build successfully!\n");

	return 0;
}

void shm_xm_exit(void)
{
	int ret = 0;

	ret = xm_dev_unregister(XM_SHM_DEV);
	
	if( ret != 0) {
		XMLOG("XtratuM SHM devices are used!\n");

		return;
	}

	ret = xm_shmpool_destroy();

	if (ret != 0) {
		XMLOG("XtratuM SHM devices are used!\n");

		return;
		
	}

	XMLOG("XtratuM SHM devices are destroyed!\n");

}

module_init(shm_xm_init);
module_exit(shm_xm_exit);

