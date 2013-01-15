/*
 * $FILE: xmdev.c
 * 	
 * Description:
 * 	xtratum device file
 *
 * Arthor:
 * 	Bai Shuwei <baishuwei@dslab.lzu.edu.cn>
 * */

#include <linux/kernel.h>
//#include <linux/version.h>
#include <linux/module.h>

#include <domain.h>
#include <xmdev.h>

static xm_dev_t *devs[XM_DEVS_NUM];
static unsigned long devmask;

int xm_dev_register(int devid, xm_dev_t *dev)
{
	if (devid >= XM_DEVS_NUM || devid < 0) {
		return -1;
	}

	/* the devid has been taken up */
	if(devmask & (1 << devid)) {
		return -1;
	}

	devmask |= 1 << devid;

	devs[devid] = dev;

	return 0;
}

EXPORT_SYMBOL(xm_dev_register);

int xm_dev_unregister(int devid)
{
	if (devid >= XM_DEVS_NUM || devid < 0) {
		return -1;
	}

	/* Some system use the device,
	 * Connot release the device*/
	if (devs[devid] == NULL || devs[devid]->client > 0) {
		return -1;
	}

	devmask &= ~(1 << devid);
	devs[devid] = NULL;

	return 0;	
}

EXPORT_SYMBOL(xm_dev_unregister);

xm_dev_t *xm_get_dev(int devid)
{
	if (devid >= XM_DEVS_NUM || devid < 0) {
		return NULL;
	}

	if (devmask & (1 << devid)) { 
		return devs[devid];
	}

	return NULL;
}

EXPORT_SYMBOL(xm_get_dev);

int xm_dev_ioctl(unsigned long dev, 
		unsigned long cmd, void *arg)
{
	
	unsigned long devid;
	unsigned long minor;
	int ret;

	devid = (dev >> 16) & 0xFFFF;
	minor = dev & 0xFFFF;

	if(devid > XM_DEVS_NUM) {
		return -ENODEV;
	}

	if(!(devmask & (1 << devid))) {
		return -ENOTTY;
	}

	ret = devs[devid]->dev_ioctl_handler(minor, cmd, arg);

	return ret;
}

EXPORT_SYMBOL(xm_dev_ioctl);
