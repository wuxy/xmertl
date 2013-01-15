// uart testing utility 
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "regression.h"

// files to test
struct {
	char *dev;
	int fd;
} ufile[] = {
	{dev: "/dev/uart/eia%dctl"},
	{dev: "/dev/uart/eia%d"},
	{dev: "/dev/uart/eia%dstatus"},
	{NULL},
};

enum
{
	Oread,
	Owrite,
};

int
openuart(char *dev){
	int fd;

	if ((fd = open(dev, O_RDWR, 0)) < 0){
		printf("error: open: %s\n", strerror(errno));
		return -1;
	}
	return fd;
}

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
	int fileop, ret = 0;
	char buf[128];
	char udev[128];

	// parse args
	nuart = 0;
	if (argc < 4)
		return -1;

	nuart = atoi (argv[1]);
	if (strncmp(argv[2], "r", 1) == 0)
		fileop = Oread;
	else if (strncmp(argv[2], "w", 1) == 0)
		fileop = Owrite;
	else
		return -1;

	// open files and configure uartn
	for (ft=0; ufile[ft].dev != NULL; ft++){
		snprintf(udev, sizeof(udev), ufile[ft].dev, nuart);
		if ((ufile[ft].fd = openuart(udev)) < 0)
			return -1;
		printf ("open %s fd:%d\n", udev, ufile[ft].fd);
	}	

	ret = writestr(ufile[0].fd, "b9600 l8 s1 pn f1");
	ret = readstr(ufile[0].fd, buf, sizeof(buf));
	printf("ctl: %s\n", buf);

	ret = readstr(ufile[2].fd, buf, sizeof(buf));
	printf("stat: %s\n", buf);

	switch (fileop){
	case Oread:
		ret = readstr(ufile[1].fd, buf, sizeof(buf));
		printf("read: %s\n", buf);
		break;

	case Owrite:
		ret = writestr(ufile[1].fd, argv[3]);
		printf("write: %s\n", argv[3]);
		break;
	}

	for (ft=0; ufile[ft].dev != NULL; ft++)
		close (ufile[ft].fd);
	return 0;
}
