#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define fifo_dev "/dev/rtf1"

int main(int argc, char *argvs[])
{
	char message[] = {"123456789012345678901234567890"};
	int fd;
	int ret;

	if((fd = open(fifo_dev, O_RDWR, O_EXCL)) < 0) {
		printf("open device error\n");
		return 0;
	}	

	ret = write(fd, message, 20);

	close(fd);
	
	return 0;
}
