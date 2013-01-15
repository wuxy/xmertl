/*
 * $FILE: dev.c
 *
 * devices
 *
 * $VERSION: 1.0
 *
 * Author: Salvador Peiró <speiro@ai2.upv.es>
 *
 * $LICENSE:  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <dev.h>
#include <stdio.h>
#include <string.h>

void
mkqid(Qid *q, vlong path, ulong vers, int type)
{
	q->type = type;
	q->vers = vers;
	q->path = path;
}

int
init_devs(void) {
	int i, ret;

	ret = 0;
	for (i=0; devtab[i]; i++){
    		printf ("\t- init %s: ", devtab[i]->name);
		ret = devtab[i]->init();
    		if (ret == 0)
		      printf ("ok\n");
		else
		      printf ("ko\n");
	}
	return ret;
}

int
devno (int dc) {
	int i;

	for (i=0; devtab[i] != NULL; i++)
		if (devtab[i]->dc == dc)
			return i;
	return -1;
}

int
readstr(unsigned long off, char *buf, unsigned long n, char *str)
{
	int size;

	size = strlen(str);
	if(off >= size)
		return 0;
	if(off+n > size)
		n = size-off;
	memmove(buf, str+off, n);
	return n;
}

// generic device functions
int
devinit(void) {
	return 0;
}

int
devopen (char* name, int omode) {
	return 0;
}

long
devread (int fd, void* str, long n, vlong offset) {
	return 0;
}

long
devwrite (int fd, void* str, long n, vlong offset) {
	return 0;
}

void
devclose (int fd) {
}
