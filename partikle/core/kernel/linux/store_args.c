#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

enum { MAXARGS = 16 };
int progargc;
char *progargv[MAXARGS];

// this function takes the program parameters
// and stores them so applications can access themu
int
store_args (int argc, ...){
	va_list ap;
	char *arg;
	int i = 0;

	if (argc>MAXARGS)
		return -1;
	progargc = argc;
	va_start (ap, argc);
	for (;;){
		if ((arg = va_arg(ap, char *)) == NULL)
			break;
		progargv[i++]=arg;
	}
	progargv[i] = NULL;
	return 0;
}
