/*
 * $FILE:
 * 	luart.c
 *
 * Dscription:
 * 	this is the linux uart device driver
 *
 * Author:
 * 	Bai Shuwei <baishuwei@dslab.lzu.edu.cn>
 *
 * */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/gfp.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>
#include <asm/string.h>
#include <arch/mm.h>

#include <serial.h>
#include <xmdev.h>
#include "luart.h"

static uart_dev_t uart = {4, 0x3f8, 0, 9600};
static rt_serial_t serial;

/* device number for xtratum, cannot be changed */
//static int xmajor = XM_SERIAL_DEV; 

static int minor = 15;	/* Local uart */
static int ruart = 15;	/* Remote uart */

static int lumajor = 240;	/* Major number used by linux*/
static char luname[] = {"luart"};

static int clients;

static struct semaphore wsem;

module_param(ruart, int, 0);
module_param(minor, int, 0);

module_param(lumajor, int, 0);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bai Shuwei<baishw@dslab.lzu.edu.cn>");
MODULE_DESCRIPTION("Linux UART Serial Device for XtratuM!");

static int rt_serial_user_read(dom_fifo_t *df, 
		char __user *dest, size_t size)
{
	serial_fifo_t *ff;
	long bom, offset, nbom;
	long ret;
	int rsize;

	if(df == NULL || dest == NULL || size < 0)
		return -EINVAL;
	
	ff = df->serf;
	do {
		rsize = (ff->top + ff->size - ff->bom) % ff->size;

		if(rsize > size) rsize = size;

		bom = ff->bom;
		nbom = (bom + rsize) % ff->size;

		if ((ff->top < bom) && (rsize > ff->size - ff->bom)) {
			offset = ff->size - bom;
			ret = copy_to_user(dest, df->addr + bom, offset);
			ret = copy_to_user(dest + offset, df->addr, rsize - offset);
		} else {
			ret = copy_to_user(dest, df->addr + bom, rsize);
		}

	} while (CAS(&ff->bom, bom, nbom) != bom);

	return rsize;
}

static int rt_serial_user_write(dom_fifo_t *df, 
		const char __user *src, size_t size)
{
	serial_fifo_t *ff;
	long top, offset, ret;
	int wsize;

	if (df == NULL || src == NULL || size < 0)
		return -EINVAL;

	ff = df->serf;

	wsize = (ff->bom + ff->size - ff->top - 1) % ff->size;

	if (wsize < size) return -ENOMEM;

	wsize = size;
	top = ff->top;

	if ( (ff->bom <= top) && (ff->size - top < wsize)) {
		offset = ff->size - top;
		ret = copy_from_user(df->addr + top, src, offset);
		ret = copy_from_user(df->addr, src+offset, wsize-offset);
	} else {
		ret = copy_from_user((void *)(df->addr + top), src, wsize);
	}

	
	ff->top = (ff->top + wsize) % ff->size;

	return wsize;
}

static int linux_uart_read(struct file *fp, 
	char __user *buff, size_t size, loff_t *off)
{
	int rsize;

	if(buff == NULL) return 0;
	
	if(fp->private_data == NULL) return -EPERM;

	if( !is_bit_set(serial.out.serf->flag, RT_SERIAL_dom_READ)) {
		return -EPERM;
	}

	rsize = rt_serial_user_read(&serial.in, buff, size);

	return rsize;	
		
}

static int linux_uart_write(struct file *fp,
	const char __user *buff, size_t size, loff_t *off)
{
	int wsize;

	if(buff == NULL) return 0;
	
	if(fp->private_data == NULL) return -EPERM;

	if( !is_bit_set(serial.out.serf->flag, RT_SERIAL_dom_WRITE)) {
		return -EPERM;
	}

	down_interruptible(&wsem);
	
	wsize = rt_serial_user_write(&serial.out, buff, size);

	up(&wsem);

	if(wsize > 0) {
		outb(0xB, UART_IER(uart));
		outb(0x9, UART_MCR(uart));
	}

	return wsize;	
}

static int linux_uart_open(struct inode *np, struct file *fp)
{
	int min = MINOR(np->i_rdev);

	if (min != 0 || serial.in.addr == NULL) {
		return -ENODEV;
	}
	
	if( !is_bit_set(serial.in.serf->flag, RT_SERIAL_dev_INIT) )
		return -EPIPE;

	if(fp->private_data == &serial) return 0;

	clients ++;
	fp->private_data = &serial;	

	return 0;
}
	
static int linux_uart_release(struct inode *np, struct file *fp)
{
	int min = MINOR(np->i_rdev);

	if (min != 0 || serial.in.addr == NULL) {
		return -ENODEV;
	}

	if(fp->private_data != &serial) return -EPERM;

	clients --;
	if(clients == 0) fp->private_data = NULL;	

	return 0;
}
	
static int linux_uart_ioctl(struct inode *np, 
	struct file *fp, unsigned int cmd, unsigned long arg)
{

	return 0;
}

struct file_operations uart_fops = {
	read:	linux_uart_read,
	write:	linux_uart_write,
	open:	linux_uart_open,
	release:linux_uart_release,
	ioctl:	linux_uart_ioctl, };

static int vuart_init(void)
{
	int ret;
	
	ret = rt_console_get(minor, (unsigned long *)&serial.in.serf, 
			(unsigned long *)&serial.out.serf);

	if (ret) {
		return ret;
	}

	serial.out.serf->flag |= ((ruart << 24) & 0xFF000000);
	serial.in.serf->flag |= ((ruart << 24) & 0xFF000000);
	

	//if( !is_bit_set(serial.in.serf->flag, RT_SERIAL_dev_INIT) )
	//	return -EPIPE;

	set_bit(serial.out.serf->flag, RT_SERIAL_dom_USED);
	set_bit(serial.in.serf->flag, RT_SERIAL_dom_USED);

	serial.in.addr = serial.in.serf->addr;
	serial.out.addr = serial.out.serf->addr;

	return  0;
}

static int vuart_eixt(void)
{
	rt_console_free(minor);

	return 0;
}

static int __init linux_uart_init(void)
{
	int ret;

	ret = register_chrdev(lumajor, luname, &uart_fops);

	if (ret < 0) {
		printk("linux uart register failed!\n");
		return -1;
	}

	ret = vuart_init();

	if (ret < 0) {
		printk("linux uart initialization failed %d!\n", ret);
		ret = unregister_chrdev(lumajor, luname);
		return -1;
	}

	init_MUTEX(&wsem);
	
	printk("<XtratuM> Linux UART register Suggested!\n");

	return 0;
}

static void __exit linux_uart_exit(void)
{
	int ret;

	vuart_eixt();

	ret = unregister_chrdev(lumajor, luname);

	printk("<XtratuM> Linux UART Exit!\n");
}

module_init(linux_uart_init);
module_exit(linux_uart_exit);
