#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define fifo_dev "/dev/rtf10"

int main(int argc, char *argvs[])
{
        char message[] = {"1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
        int fd;
	int ret;
        int i;
	int total = 0;

        if((fd = open(fifo_dev, O_RDWR, O_EXCL)) < 0) {
                printf("open device error\n");
        }
	//message[39] = '\0';
	for(i = 0; i< 1; i++) {
		ret = write(fd, message, 62);
        	if(ret <= 0) break;
		printf("ret = %d\n", ret);
		total += ret;
	}
	printf("total = %d\n", total);
	

	close(fd);

        return 0;
}


