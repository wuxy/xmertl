#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
//#include <sys/types.h>
//#include <sys/stat.h>


#define mbuff_dev "/dev/shm1"

int main(int argc, char *argvs[])
{
	char *mbuf;
    int fd;
	int size = 1024;
	int i;
        
	if((fd = open(mbuff_dev, O_RDWR)) < 0) {
		printf("open device error\n");
	}

	mbuf = mmap(NULL, size, PROT_WRITE, MAP_SHARED|MAP_FILE, fd, 0);
	
	if(mbuf == NULL) {
		printf("MMAP FAILED!\n");
		close(fd);
		return -1;
	}

	close(fd);

	for(i = 0; i < size; i++) {
		mbuf[i] = 'c';
	}	
	//close(fd);

    return 0;
}


