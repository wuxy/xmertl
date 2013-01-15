#include <stdio.h>
#include <setjmp.h>
#include "regression.h"

int main(int argc, char *argv[]) {
	jmp_buf	 env;

	printf("setjmp: ");
	if (setjmp (env) == 2) {
		printf("ok\n");
		return TEST_OK;
	}
	longjmp(env, 2);
	printf("ko\n");
	return TEST_FAIL;
}
