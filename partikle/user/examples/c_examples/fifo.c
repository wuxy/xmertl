#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define fifo_dev "/dev/rtf0"

int main(int argc, char *argv[])
{
	char msg[] = "I am in the partikle\n";
	int fd;
	int i, ret;

	if((fd = open(fifo_dev, O_RDWR, O_EXCL)) < 0) {
		printf("open device error %d\n", errno);
		return 0;
	}	
	
//	write(fd, msg, sizeof(msg));
	for(i = 0; i < 10; i ++) {
		ret = write(fd, msg, sizeof(msg));
		printf("write fd %d ret = %d\n", fd, ret);
		if(ret != sizeof(msg))
			return -1;
	}
	
	return 0;
}
