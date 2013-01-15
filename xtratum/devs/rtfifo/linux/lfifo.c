/*
 * $FILE: lfifo.c
 * 
 * Linux FIFO for RTLinux 
 *
 * $VERSION: 1.0
 *
 * Author: Bai Shuwei <baishw06@lzu.cn> 
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
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <asm/semaphore.h>
#include <linux/fs.h> 
#include "lfifo.h"

static struct linux_fifo_struct lfifos[FIFO_COUNT] = {{0, 0, 0, NULL}, };

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bai Shuwei<baishw06@lzu.cn>");
MODULE_DESCRIPTION("Linux FIFO Device Driver");

static ssize_t lfifo_read(struct file *fp, char __user *buff, size_t size, loff_t *off)
{
	linux_fifo_t ff;
	int ret = 0;
	int index = -1;

	if(buff == NULL || size < 0) {
		printk("Segmentation fault!\n");
		return -1;
	}
	if((ff = (linux_fifo_t)fp->private_data) != NULL) {
                index = ff->lf_index;
        } 

	if(index < 0 || index >= FIFO_COUNT) {
		return -ENODEV;
	}	
	//down_interruptible(&ff->sem);
	ret = rt_fifo_user_read(index, (void *)buff, size);
	//up(&ff->sem);	
	// if ret == -1, the fifo is empty.

	return ret;
}

static ssize_t lfifo_write(struct file *fp, const char __user *buff, size_t size, loff_t *off)
{
        linux_fifo_t ff;
        int ret;
        int index = -1;

        if(buff == NULL || size < 0) {
                printk("Segmentation fault!\n");
	        return 0;
        }
	if((ff = (linux_fifo_t)fp->private_data) != NULL) {
		index = ff->lf_index;
	} 
        if(index < 0 || index >= FIFO_COUNT) {
                return -ENODEV;
        }

	//down_interruptible(&ff->sem);
        ret = rt_fifo_user_write(index, buff, size);

//	up(&ff->sem);
	// if ret == -1, the fifo is full.
        /*unlock*/
        return ret;
}

static int lfifo_open(struct inode *np, struct file *fp)
{
	linux_fifo_t ff;
	int num = MINOR(np->i_rdev);
	
	/* lock */
//	down_interruptible(&ff->sem);
	ff = (linux_fifo_t) fp->private_data;
	if(!ff) {
		if(num >= FIFO_COUNT) {
			/*unlock*/
//			up(&ff->sem);
			return -ENODEV;
		}
		ff = &lfifos[num];
		ff->lf_users++;
		fp->private_data = ff;
	}
//	up(&ff->sem);
	/*unlock*/
	
	return 0;
}

static int lfifo_release(struct inode *np, struct file *fp)
{
        linux_fifo_t ff;
        int num = MINOR(np->i_rdev);

	/*lock*/
//	down_interruptible(&ff->sem);
        ff = (linux_fifo_t) fp->private_data;
        if(ff) {
                if(num >= FIFO_COUNT) {
			/*unlock*/
//			up(&ff->sem);
			return -ENODEV;
		}
		ff->lf_users--;
                fp->private_data = NULL;
        }
//	up(&ff->sem);
	/*unlock*/
        return 0;
}

static int lfifo_ioctl(struct inode *np, struct file *fp, 
			unsigned int cmd, unsigned long para)
{
	int num = MINOR(np->i_rdev);
	void *pp = (void *)para;
	linux_fifo_t ff;
	int ret = 0;

	/* lock */

	ff = (linux_fifo_t) fp->private_data;
	if(ff) {
//		down_interruptible(&ff->sem);
		ret = rt_fifo_user_ioctl(num, cmd, (void *)pp);
//		up(&ff->sem);
	}
	
	return 0;
} 

struct file_operations fifo_fops = {
		read: lfifo_read,
		write: lfifo_write,
		open: lfifo_open,
		release: lfifo_release,
		ioctl: lfifo_ioctl,};

#define FIFO_MAJOR 150 
#define FIFO_NAME "rtf"
static int __init lfifo_init(void)
{
	int ret;

	if((ret = register_chrdev(FIFO_MAJOR, FIFO_NAME, &fifo_fops)) < 0) {
		printk("<XtratuM> Linux Register FIFO device failed \n");
		return -1;
	}
	
	for(ret = 0; ret < FIFO_COUNT; ret++) {
		lfifos[ret].lf_index = ret;	
		sema_init(&lfifos[ret].sem, 1);
	}
	printk("<XtratuM> Linux Register FIFO device successful \n");

	return 0;	
}

static void __exit lfifo_exit(void)
{
	int ret;
	ret = unregister_chrdev(FIFO_MAJOR, FIFO_NAME);

	printk("<XtratuM> Linux Unregister FIFO device successful\n");
}

module_init(lfifo_init);
module_exit(lfifo_exit);


