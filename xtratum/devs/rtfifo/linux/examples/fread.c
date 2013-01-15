#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define fifo_dev "/dev/rtf1"

int main(int argc, char *argvs[])
{
        char string[40];
        int fd;
	int ret;
        int i, j;
	int total = 0;
        
	if((fd = open(fifo_dev, O_RDWR, O_EXCL)) < 0) {
                printf("open device error\n");
        }
	string[39] = '\0';
	for(i = 0; i< 10; i++) {
		ret = read(fd, string, 39);
		if(ret <= 0) break;
		printf("%s\n", string);
		total += ret;
	}

	printf("total = %d\n", total);

	close(fd);

        return 0;
}


