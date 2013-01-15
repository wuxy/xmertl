#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "regression.h"

#define BUFSZ 128
int main (int argc, char *argv[]){
	int n, fd;
	char buf[BUFSZ];
	
	if ((fd = open ("/usr/NOTES", O_RDONLY, 0)) < 0){
		printf("open: %s\n", strerror(errno));
		return TEST_FAIL;
	}

	printf("======================= File /usr/NOTES start===========================\n");
	while ((n = read (fd, buf, BUFSZ)) > 0) {
	  write(1, buf, n);
	}
	printf("======================= File /usr/NOTES end ============================\n");
	printf("\n");
	return TEST_OK;
}
