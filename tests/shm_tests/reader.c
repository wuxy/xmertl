#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
 
#define mbuff_dev "/dev/shm/0"

int main(int argc, char *argvs[])
{
	char *mbuf;
        int fd, i;
	int size = 26;
	char c;
        
	if((fd = open(mbuff_dev, O_RDWR, 0)) < 0) {
		printf("open device error\n");
		return -1;    
	}

	mbuf = mmap(NULL, size, 0, 0, fd, 0);
	
	if(mbuf == NULL) {
		printf("MMAP FAILED!\n");
		close(fd);
		return -1;
	}

	c = 'A';
	for(i = 0; i < size; i++) {
		if (mbuf[i] != c++) {
			printf("XM/SHM testing FAILURE!\n");
			break;
		}
	}	

	close(fd);
	
	return 0;
}


