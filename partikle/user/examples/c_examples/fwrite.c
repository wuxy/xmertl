#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define fifo_dev "/dev/rtf1"

int main(int argc, char *argvs[])
{
	char message[] = {"123456789012345678901234567890"};
	int fd;
	int i, ret;

	if((fd = open(fifo_dev, O_RDWR, O_EXCL)) < 0) {
		printf("open device error\n");
		return 0;
	}	

//	write(fd, message, 22);
	for(i = 0; i < 50; i ++) {
		ret = write(fd, message, 20);
		printf(" i= %d\n", i);
		if(ret <= 0) break;
	}
	
	return 0;
}
