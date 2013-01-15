/*
 * #FILE: string.c
 *
 * Author:
 * 	Bai Shuwei<baishuwei@dslab.lzu.edu.cn>
 *
 * License:
 * 	GPL 2.0V
 * */
#include <doms/stdlib.h>
#include <doms/string.h>


void *memcpy(void *dest, const void *src, int n)
{
	unsigned char *dst = (unsigned char *)dest;
	const unsigned char *sc  = (unsigned char *)src;

	if(dst == NULL || sc == NULL)
		return NULL;

	while(n--) {
		*dst = *sc;
		dst ++;
		sc ++;
	};

	return dest;
}
