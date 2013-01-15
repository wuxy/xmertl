#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define fifo_dev "/dev/rtf1"

int main(int argc, char *argvs[])
{
	char message[] = {"123456789012345678901234567890"};
	char buff[50];
	int fd;
	int i, ret;

	if((fd = open(fifo_dev, O_RDWR, O_EXCL)) < 0) {
		printf("open device error\n");
		return 0;
	}	
	
	ret = read(fd, buff, 20);

	if (ret != 20) {
		printf("XM/FIFO testing FAILURE!\n");
		close(fd);
		return 0;
	}

	for(i = 0; i < 20; i++) {
		if (buff[i] != message[i]) {
			printf("XM/FIFO testing FAILURE!\n");
			close(fd);
			return 0;
		}
	}

	printf("XM/FIFO testing PASS!\n");
	
	close(fd);
	
	return 0;
}
