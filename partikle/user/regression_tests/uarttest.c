// A Hello World (tm) example 2

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "regression.h"

// files to test
char * file[] = {
	"/dev/uart/eia%dctl",
	"/dev/uart/eia%d",
	"/dev/uart/eia%dstatus",
	NULL,
};

int
writestr(int fd, char *str){
	int n;
	int clen = strlen(str);
	
	n = write (fd, str, clen);
	if (n != clen)
		return -n;
	return n;
}

int
readstr(int fd, char *str, int maxn){
	int off=0;
	int i, n;

	for (i=0; i<maxn; i++) // clean str
		str[i]='\0';
	while ((n = read (fd, str+off, maxn-off)) > 0){
		off+=n;
  	}
	return off;
}

int
main (int argc, char **argv) {
	int nuart, ft;
	int cfd, ret = 0;
	char dev [128];
	char buf[128];
	
	for (nuart=0; nuart < 2; nuart ++){
		for (ft=0; file[ft] != NULL; ft++){
			snprintf(dev, sizeof(dev), file[ft], nuart);
			if ((cfd = open(dev, O_RDWR, 0)) < 0){
				printf("error: open \"%s\": %s\n", dev, strerror(errno));
				return TEST_FAIL;
			}
			printf ("open %s fd:%d\n", dev, cfd);
			
			switch(ft){
			case 0:
				ret = writestr(cfd, "b38400 l8 s1 pe f1 d0");
				ret = readstr(cfd, buf, sizeof(buf));
				printf("ctl: %s\n", buf);
				break;
			case 1:
				ret = writestr(cfd, "Hello World!!!\n");
				ret = readstr(cfd, buf, sizeof(buf));
				printf("data: %s\n", buf);
				break;
			case 2:
				ret = readstr(cfd, buf, sizeof(buf));
				printf("stat: %s\n", buf);
				break;
			}
 			
			close (cfd);
			if (ret < 0)
				return TEST_FAIL;
		}
	}
	return TEST_OK;
}
