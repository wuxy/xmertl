/*
 * $FILE:
 * 	xmdev.h
 *
 * Description:
 * 	
 * Author:
 * 	Bai Shuwei <baishuwei@dslab.lzu.edu.cn>
 * */

#ifndef __XM_DEVS_H__
#define __XM_DEVS_H__

/* Don't change the value, unless you know what they are */
#define XM_FIFO_DEV 	0
#define XM_SHM_DEV  	1
#define XM_SERIAL_DEV 	2

#define XM_DEVS_NUM 32  /* The count of device types */

#define XM_DEV_NUM  16	/* Maximum minor number */

#define XM_SERIAL_DEV_NAME "serial.xmd"

/* Device IOCTL COMMAND */

#define XMDEV_IO_OPEN   0x01
#define XMDEV_IO_CLOSE  0x02
#define XMDEV_IO_READ   0x04
#define XMDEV_IO_WRITE  0x08
#define XMDEV_IO_FLUSH  0x10
#define XMDEV_IO_WKUP   0x20
#define XMDEV_IO_SLEEP  0x40
#define XMDEV_IO_DEL    0x80
#define XMDEV_IO_INIT   0x100
#define XMDEV_IO_DEV	0x200

/* XM SHM DEVICE IOCTL COMMAND */
#define XM_SHM_CREATE   0x01
#define XM_SHM_TRUNCATE 0x02 
#define XM_SHM_MAP      0x04 
#define XM_SHM_UNMAP    0x08
#define XM_SHM_REMOVE   0x10
#define XM_SHM_EXIST    0x20
#define XM_SHM_VADDR    0x40
#define XM_SHM_SIZE     0x80


typedef struct
xm_dev_struct
{
	int devid;
	int (*dev_map_handler)(unsigned long, unsigned long, 
			unsigned long, void *);
	int (*dev_unmap_handler)(unsigned long, unsigned long, 
			unsigned long);
	int (*dev_ioctl_handler)(unsigned long minor, 
			unsigned long cmd, void *);
	int client;
} xm_dev_t;

extern int xm_dev_ioctl(unsigned long minor, unsigned long cmd, void *arg);
extern int xm_dev_register(int devid, xm_dev_t *dev);
extern int xm_dev_unregister(int devid);
extern xm_dev_t *xm_get_dev(int devid);

#if 0
#define CAS(adr, ov, nv) ({ \
	__typeof__(ov) ret; \
	__asm__ __volatile__( \
	"cmpxchg %3,%1" \
	:"=a"(ret),"+m"(*(volatile unsigned int *)(adr)) \
	:"a"(ov),"r"(nv)); \
	ret; \
})
#endif

#define is_serial_dev(d) (strcmp(XM_SERIAL_DEV_NAME, d->name) == 0)

#endif
