#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "ashm.h"
 
#define mbuff_dev "/dev/shm0"

int main(int argc, char *argvs[])
{
	char *mbuf;
	int size = 1024;
	int i;
        

	mbuf = mbuff_alloc(mbuff_dev, size); 
	if(mbuf == NULL) {
		printf("MMAP FAILED!\n");
		return -1;
	}

	for(i = 0; i < size; i++) {
		printf("%c", mbuf[i]);	
	}

	printf("\n");
	mbuff_free(mbuf, size);	
	
	//close(fd);
        return 0;
}


