#include <stdio.h>
#include <fcntl.h>

int main (int argc, char *argv[]){
	int fd;
	FILE f; 
	
	fprintf(stdin, "stdin: foo\n");
	fprintf(stdout, "stdout: foo\n");
	fprintf(stderr, "stderr: foo\n");
	
	fd = open("/dev/test", O_RDONLY, 0);
	if (fd < 0)
		return -1;
	f.fd = fd;
	fprintf(&f, "write to test\n");
	
	return 0;
}
