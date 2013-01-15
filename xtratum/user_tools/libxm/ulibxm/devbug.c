/*
 * $FILE:
 *  devbug.c
 *
 * Description:
 *  xtratum device debug file
 *
 * Author: 
 *  Bai Shuwei <baishuwei@dslab.lzu.edu.cn>
 *
 * */

#include <xm_syscalls.h>
#include <devbug.h>

static char digits[] = {"0123456789abcdef"};

int hexprint(unsigned int val)
{	
	unsigned int hx = val;
	char hex[10];
	int i, j;

	if ( val == 0) {
		write_scr("0x0\n", 4);
		return 0;
	}

	for ( i = 9; (i > 1) && (hx); i--) {
		hex[i] = digits[hx % HEX];
		hx = hx >> 4;
	}

	hex[i] = 'x';
	hex[i-1] = '0';

	for (j = 0, i--; i < 10; i++, j++) {
		hex[j] = hex[i];
	}
	
	write_scr(hex, j);

	write_scr("\n", 1);

	return 0;
}


