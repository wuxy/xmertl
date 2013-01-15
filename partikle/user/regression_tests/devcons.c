#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "regression.h"

#define BUFSZ 128
int main (int argc, char *argv[]){
	int n, tfd;
	char buf[BUFSZ];
	
	if ((tfd = open ("/dev/console/drivers", O_RDONLY, 0)) < 0){
		printf("open: %s\n", strerror(errno));
		return TEST_FAIL;
	}

	while ((n = read (tfd, buf, BUFSZ)) > 0) {
		printf("# %d = read (%d, %x, %d)\n", n, tfd, buf, BUFSZ);
		printf("%s", buf);
	}
	close (tfd);
	return TEST_OK;
}
