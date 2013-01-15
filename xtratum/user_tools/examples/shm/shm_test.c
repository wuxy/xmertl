#include <xm/xmdev.h>
#include <xm_syscalls.h>
#include <devbug.h>

int kmain (void) {
  int index;
  unsigned long dev;
  int err;

  //  a[8000]=10;

  unsigned long SHM_ADDR = 64*1024*1024;
  unsigned long *data = (unsigned long *) SHM_ADDR;
  
  index = shm_open("shm0", 0, 0);
  hexprint(index);
  
  if (index < 0 || index >= 16) {
  	write_scr("wrong shm_open\n", 15);
    return -1;
  }

  dev = ((XM_SHM_DEV << 16) & 0xFFFF0000) |
		(index & 0x0000FFFF);

  if((err = dev_ctl(dev, XM_SHM_MAP, data)) == 0) {
     data[0] = 0xFefefefe;
  	 write_scr("in dev\n", 7);
     hexprint(data[0]);
     dev_ctl(dev, XM_SHM_UNMAP, data);
  } else {
  	hexprint(err);
  }

  shm_unlink("shm0");


  //return a [4];
  return 0;
}
