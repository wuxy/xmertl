#include <config.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fs.h>
// as it's name says, this is just a minimal test device 

static int
testopen (char * name, int omode) {
	int fd;
	
	if ((fd = newfd()) >= MAX_FILES){
		SET_ERRNO(ENOMEM);
		return -1;
	}
	files[fd].type = devno('T');
	printf("testopen fd: %d name: %s\n", fd, name);
	return fd;
}

static long
testread (int fd, void *str, long n, vlong offset) {
	printf("testread fd: %d str: %s\n", fd, str);
	return 0;
}

static long
testwrite (int fd, const void *str, long n, vlong offset) {
	printf("testwrite fd: %d str: %s\n", fd, str);
	return 0;
}

void
testclose (int fd) {
	freefd(fd);
}

Dev testdevtab = {
	'T',
	"test",
	
	devinit,
	testopen,
	testread,
	testwrite,
	testclose,
};

