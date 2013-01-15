#include <stdio.h>

int
main (int argc, char *argv[])
{
	int i = 0;
	printf ("argc: %d (%#X), i %d (%#X)\n", argc, &argc, i, &i);
	for (i=0; i < argc; i++)
		printf("argv[%d] = %s\n", i, argv[i]);

	if (argv[i] != NULL)
		return -1;
	return 0;
}
