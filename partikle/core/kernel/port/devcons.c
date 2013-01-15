/*
 * $FILE: devcons.c
 *
 * console driver
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

#include <config.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fs.h>
#include "cons.h"

#define NELEM(ary) (sizeof(ary)/sizeof(ary[0]))
extern struct File files [MAX_FILES];
extern int newfd();
extern int freefd(int fd);

enum
{
	Qcons,
	Qdrivers,
	Qkprint,
	Qnull,
        Qsysctl,
};

Dirtab contab[] =
{
	{"cons",		{Qcons},	0,	0666},
	{"drivers",	{Qdrivers},	0,	0444},
	{"kprint",	{Qkprint},	0,	0444},
	{"null",		{Qnull},	0,	0666},
	{"sysctl",       {Qsysctl},      0,      0644,}
};

static int consopen(char * name, int omode);
static int
consinit(void){
	int i, fd;

	// stdin, stdout & stderr point to /dev/console
	for (i=0; i < 3; i++){
		fd = consopen ("/dev/console/cons", 0);
		if (fd != i)
		return -1;
	}
	return 0;
}

static int
consopen(char * name, int omode) {
	int i, fd;
	char * pn;

	pn = name + strlen("/dev/console/");
	for (i=0; i < NELEM(contab); i++)
		if (strcmp (pn, contab[i].name) == 0)
			break;
	if (i == NELEM(contab)){
		SET_ERRNO(EBADF);
		return -1;
	}
	if ((fd = newfd()) >= MAX_FILES){
		freefd(fd);
		SET_ERRNO(ENOMEM);
		return -1;
	}
	files[fd].type = devno('c');
	files[fd].qid = contab[i].qid;
	files[fd].pos = 0;

	return fd;
}

#define DRIVERSZ	(KNAMELEN*11)
int
listdevices (char * devices){
	unsigned long i;
	int nlen, n;
	char *p;

	p = devices;
	for (i=0; i < ndevs && devtab[i]; i++) {
		nlen = strlen(devtab[i]->name);
		if ((p - devices +nlen+5) > DRIVERSZ)
			return -1; // not enough space

		n = snprintf(p, nlen+5, "#%c %s\n", devtab[i]->dc, devtab[i]->name);
		p += n;
		if (n<=0)
			return -1;
	}
	
	return p-devices;
}

static long
consread(int fd, void *str, long n, vlong offset) {
	struct File * f = &files[fd];
	char devices[DRIVERSZ];
	int len;

	switch(f->qid.path){
	default:
		SET_ERRNO(ENOENT);
		return -1;

	case Qcons:
		return read_cons (fd, str, n);

	case Qdrivers:
		len=listdevices(devices);
		if (offset < 0 || offset >= len)
			return 0;
		if (offset+n > len)
			n = len - offset;
		if (memmove (str, devices+offset, n))
			f->pos += n;
		return n;

	case Qsysctl:
		n = readstr(offset, str, n, VERSION);
		f->pos += n;
		return n;

	case Qnull:
		return 0;
	}
}

static long
conswrite(int fd, const void *str, long n, vlong offset) {
	struct File f = files[fd];

	switch(f.qid.path){
	default:
		SET_ERRNO(ENOENT);
		return -1;

	case Qcons:
		return write_cons (fd, str, n);

	case Qdrivers:
		SET_ERRNO(EPERM);
		return -1;

	case Qnull:
		break;

	}
	return n;
}

Dev consdevtab = {
	'c',
	"console",
	
	consinit,
	consopen,
	consread,
	conswrite,
	devclose,
};

