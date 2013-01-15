// A Hello World (tm) example 2

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "regression.h"

static int
substn (char * src, int n, int val){
	char *p;
	if (strlen(src) < n)
		return -1;

	p = src;
	while (n-- > 0)
		p++;
	*p = (char) val;
	return 0;
}
	
int main (int argc, char **argv) {
	int i;
	int cfd, n;
	char * buf = "Hello World!!!\n";
	char dev [] = "/dev/test/0";

	for (i=0; i < 10; i ++) {
		substn (dev, strlen(dev)-1, '0' + i); 
		if ((cfd = open(dev, O_RDWR, 0)) < 0){
			printf("constest: open \"%s\" returns %d: %s ",
				dev, cfd, strerror(errno));
			return TEST_FAIL;
		}
		printf ("open device %s fd:%d\n", dev, cfd);

 		n = write (cfd, buf, strlen(buf));
		close (cfd);
		if (n != strlen(buf))
			return TEST_FAIL;
	}
	return TEST_OK;
}
