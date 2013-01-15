#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define fifo_dev "/dev/rtf1"

int main(int argc, char *argvs[])
{
	char message[50];
	int fd;
	int i, ret;
	printf("message address = %lu\n", (unsigned long)message);

	if((fd = open(fifo_dev, O_RDWR, O_EXCL)) < 0) {
		printf("open device error\n");
		return 0;
	}	
	message[49] = '\0';
	for(i = 0; i < 5; i ++) {
		ret = read(fd, message, 49);
		if(ret <= 0) break;
		printf("%d %d %s\n", ret, i, message);
	}
	close(fd);
	
	return 0;
}
