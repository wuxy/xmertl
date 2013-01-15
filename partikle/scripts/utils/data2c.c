// data2c: taken from Inferno, and modified to not depend on it.
// author: Salvador Peiro <speiro@ai2.upv.es>
#include <stdio.h>

int
main(int argc, char *argv[])
{
	long len;
	int n;
	unsigned char block[16], *c;

	if(argc != 2){
		fprintf(stderr, "usage: data2c name\n");
		return -1;
	}
	fprintf(stdout, "unsigned char %scode[] = {\n", argv[1]);
	for(len=0; (n=read(0, block, sizeof(block))) > 0; len += n){
		for(c=block; c < block+n; c++)
			if(*c)
				fprintf(stdout, "0x%x,", *c);
			else
				fprintf(stdout, "0,");
		fprintf(stdout, "\n");
	}
	if(len == 0)
		fprintf(stdout, "0\n");	/* avoid empty initialiser */
	fprintf(stdout, "};\n");
	fprintf(stdout, "int %slen = %ld;\n", argv[1], len);
	return 0;
}
