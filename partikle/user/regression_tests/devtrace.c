#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "regression.h"

int tracectl(int tfd, char *cmd);
char * tracestat(int tfd, char *buf);

#define SLEN 64
int main (int argc, char *argv[]){
	int tfd;
	FILE tf;
	char buf[SLEN];

	if ((tfd = open ("/dev/trace/ctl", O_RDWR, 0)) < 0){
		printf("open: %s\n", strerror(errno));
		return TEST_FAIL;
	}
	tf.fd = tfd;

	if(fprintf(&tf, "enable")<0)
		return TEST_FAIL;
	printf("tracing state: %s\n", tracestat(tfd, buf));
	
	usleep(50000);

	if(fprintf(&tf, "disable")<0)
		return TEST_FAIL;
	printf("tracing state: %s\n", tracestat(tfd, buf));

	close (tfd);
	return TEST_OK;
}

char *
tracestat(int tfd, char *buf)
{
	if (read(tfd, buf, SLEN)<0){
		printf("tracestat: %s\n", strerror(errno));
		return NULL;
	}
	return buf;
}
