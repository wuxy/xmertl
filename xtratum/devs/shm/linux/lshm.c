/*
 * $FILE:  
 *
 * $Description:
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>


#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/fs.h>

#include "../shm.h"
#include "lshm.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bai Shuwei<baishuwei@weslab.com>");
MODULE_DESCRIPTION("LSHM DEIVCES MANAGEMENT MODULE");

static lshm_dev_t shm_dev;


static int lshm_minor_to_name(int minor, char *name)
{
	int index = minor;
	int i = 0;

	while(name[i]) { i++;}

	if (index == 0) name[i] = '0';
	else {
		while(index) {
			name[i] = (index % 10) + '0';
			index   = index / 10;
			i ++;
		}
	}

	return 0;

}

static int lshm_open(struct inode *inode, struct file *pf)
{
	int minor = MINOR(inode->i_rdev);
	struct lshm_t *shm;
	int xmminor, ret;
	unsigned long arg;
	char name[10] = "shm";

	if(minor >= LINUX_SHM_COUNT) return -ENODEV;

	shm = (struct lshm_t *) pf->private_data;

	if(NULL == shm) {

		lshm_minor_to_name(minor, name);
		xmminor = xm_shm_open(name, 0UL, 0UL);
		
		if (xmminor < 0 || xmminor >= LINUX_SHM_COUNT) return -EIO;

		ret = xm_shm_ioctl(xmminor, XM_SHM_SIZE, &arg);
							
		if(ret) {
			xm_shm_unlink(shm_dev.lshm[minor].shm_xmminor);
			return ret;
		}

		shm_dev.lshm[minor].shm_size = arg;

		ret = xm_shm_ioctl(xmminor, XM_SHM_VADDR, &arg);
							
		if(ret) {
			xm_shm_unlink(shm_dev.lshm[minor].shm_xmminor);
			return ret;
		}
		shm_dev.lshm[minor].shm_addr = arg;
		shm_dev.lshm[minor].shm_xmminor = xmminor;
		pf->private_data = &shm_dev.lshm[minor];
	}

	shm_dev.lshm[minor].shm_users ++;

	return 0;
}

static int lshm_release(struct inode *nd, struct file *pf)
{
	int minor = MINOR(nd->i_rdev);
	int users = 0;

	if(minor >= LINUX_SHM_COUNT) return -ENODEV;
	
	users = shm_dev.lshm[minor].shm_users ;
	
	if(--users == 0) {
		shm_dev.lshm[minor].shm_size = 0UL;
		shm_dev.lshm[minor].shm_addr = 0UL;
		pf->private_data = NULL;
	}
	
	shm_dev.lshm[minor].shm_users --;

	xm_shm_unlink(shm_dev.lshm[minor].shm_xmminor);

	return 0;
}

static ssize_t lshm_read(struct file *pf, char __user *buf, size_t size, loff_t *offset)
{
	printk("NOT PERMISSION READING DIRECT!\n");

	return 0;
}

static ssize_t lshm_write(struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("NOT PERMISSION WRITING DIRECT!\n");
	
	return 0;
}

static int lshm_mmap(struct file *pf, struct vm_area_struct *vm)
{
	struct lshm_t *shm;
	unsigned long vsize;
	unsigned long kalloc;
	char *valloc;
	int pn;

	shm = (struct lshm_t *)pf->private_data;

	if (NULL == shm) return -ENODEV;

	vsize = vm->vm_end - vm->vm_start;
	if (shm->shm_size < vsize) {
		return -EINVAL;
	}

	valloc = (char *)shm->shm_addr;

	for (pn = 0; vsize > 0; pn ++, vsize -= PAGE_SIZE) {
		kalloc = (unsigned long) page_address(vmalloc_to_page(valloc + pn*PAGE_SIZE));
		//kvaddr = kmem_kvaddr_to_kladdr(valloc);

		if(remap_pfn_range(vm, vm->vm_start + pn * PAGE_SIZE, __pa(kalloc) >> PAGE_SHIFT, PAGE_SIZE, PAGE_SHARED)) {
				printk("remap_pfn_range failed\n");
				return -EIO;
		}
	}
		
	return 0;
}

static struct 
file_operations lshm_fops = 
{
	.owner = THIS_MODULE,
	.open  = lshm_open,
	.release = lshm_release,
	.read  = lshm_read,
	.write = lshm_write,
	.mmap  = lshm_mmap,
};

#if 0
static void lshm_truncate(struct inode *inode)
{
	printk("ON THE LINE %d\n", __LINE__);
}

static struct 
inode_operations iops = 
{
	.truncate = lshm_truncate,
};

#endif

static int lshm_dev_setup(lshm_dev_t *sdev, int index)
{
	int err;
	dev_t devno;
	int dno = MKDEV(LSHM_MAJOR, index);

	if (NULL == sdev) {
		return -1;
	}

	err = alloc_chrdev_region ( &devno, LSHM_MAJOR, LINUX_SHM_COUNT, "shm");  
	if(err) {
		printk(KERN_NOTICE "ERROR <%d> ALLOC CHRDEV REGION\n", err);

		return -1;
	}


	cdev_init(&sdev->cdev, &lshm_fops);
	sdev->cdev.owner = THIS_MODULE;
	sdev->cdev.ops = &lshm_fops;
	
	err = cdev_add(&sdev->cdev, dno, LINUX_SHM_COUNT);

	if(err) {
		printk(KERN_NOTICE "ERROR <%d> ADDING SHM DEVICE\n", err);
	}

	return err;
}


int lshm_init(void)
{
	int err;

	err = lshm_dev_setup(&shm_dev, 0);

	if (err) return -1; 
	

	printk ("<XtratuM> LSHM DEVICE REGISTER SUCCESSFUL\n");

	return 0;
}



void lshm_exit(void)
{
	cdev_del(&shm_dev.cdev);

	printk ("<XtratuM> LSHM DEVICE EXIT\n");
}

module_init(lshm_init);
module_exit(lshm_exit);
