/* 
 * author: Salvador Peiro
 * calculate change in partikle's  footprint when using:
 * 1) errstr.c: including error strings (3465 bytes)
 * 2) noerrstr.c: not including them (4 bytes)
 * conclusion: if you're short of memory it can help
 */

// to run this: copy the [no]errstr.c files here
#include <stdio.h>
//#include "noerrstr.c"
#include "errstr.c" 

#define NELEM(ARY) ( (sizeof(ARY)) / (sizeof(ARY[0])) )

int main (){
	int i, size = 0;
	printf("NELEM(errstr) = %d\n",NELEM(errstr));
	printf("sizeof(errstr) = %d\n",sizeof(errstr));
	// NELEM(errstr) * 4 bytes/elem = sizeof(errstr)

	// sum _i=0 ^i=NELEM(errstr) {size(errstr[i])}
	for (i = 0; i < NELEM(errstr); i++){
		if (errstr[i] != NULL){
			printf ("errstr[%d] = %s\n", i, errstr[i]);
			size += strlen(errstr[i]) + 1;
		}
	}
	printf("errstr size: %d\n", size);
	printf("total size: %d\n", sizeof(errstr) + size );
}
