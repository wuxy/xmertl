#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

 
#define mbuff_dev "/dev/shm1"

int main(int argc, char *argvs[])
{
	char *mbuf;
        int fd;
	int size = 1024;
	int i;
        
	if((fd = open(mbuff_dev, O_RDWR)) < 0) {
		printf("open device error\n");
    	return -1;    
	}

	mbuf = mmap(NULL, size, PROT_WRITE|PROT_READ, MAP_SHARED|MAP_FILE, fd, 0);
	
	if(mbuf == NULL) {
		printf("MMAP FAILED!\n");
		close(fd);
		
		return -1;
	}

	printf("%p\n", mbuf);

	for(i = 0; i < size; i++) {
		printf("%c", mbuf[i]);	
	}	
	printf("\n");	
	
	close(fd);
	
	return 0;
}


