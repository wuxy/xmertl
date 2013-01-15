/*
 * $FILE: devuart.c
 *
 * uart driver
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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fs.h>
#include <tlsf.h>

#include "uart.h"

/*
 *  Macros to manage Qid's used for multiplexed devices
 */
#define NETTYPE(x)	(((ulong)x)&0x1f)
#define NETID(x)	((((ulong)x))>>5)
#define NETQID(i,t)	((((ulong)i)<<5)|(t))

// from netif.h
enum
{
	Nmaxaddr=	64,
	Nmhash=		31,

	Ndataqid=	1,
	Nctlqid,
	Nstatqid,
};

extern Dev uartdevtab;
extern Uart uart[];

static int uartnuart;
static Dirtab *uartdir;
static int uartndir;

#define DPRINT if (0)printf

// this should be called uartreset 
// and be called from uartinit
int
uartinit(void)
{
	int i;
	Dirtab *dp;

	for(i = 0; uart[i].hw != NULL; i++){
		if(uart[i].hw->pnp == NULL)
			continue;
		if((uart[i].hw->pnp(uart[i].hw)) == -1)
			continue;
		uartnuart++;
	}
	if (uartnuart == 0)
		return -1;
	
	uartndir = 1 + 3*uartnuart;
	uartdir = malloc_ex(uartndir * sizeof(Dirtab), memory_pool);
	dp = uartdir;
	strcpy(dp->name, ".");
	mkqid(&dp->qid, 0, 0, QTDIR);
	dp->length = 0;
	dp->perm = DMDIR|0555;
	dp++;
	
	for(i = 0; i < uartnuart; i++){
		/* 3 directory entries per port */
		sprintf(dp->name, "eia%d", i);
		mkqid(&dp->qid, NETQID(i, Ndataqid), 0, QTFILE);
		dp->perm = 0660;
		dp++;
		sprintf(dp->name, "eia%dctl", i);
		mkqid(&dp->qid, NETQID(i, Nctlqid), 0, QTFILE);
		dp->perm = 0660;
		dp++;
		sprintf(dp->name, "eia%dstatus", i);
		mkqid(&dp->qid, NETQID(i, Nstatqid), 0, QTFILE);
		dp->perm = 0444;
		dp++;
	}
	return 0;
}

static int
uartopen (char * name, int omode) {
	int i, fd;
	char *p;

	if (strlen(name)>strlen("/dev/uart/"))
		p = name + strlen("/dev/uart/");
	else
		return -1;

	for(i=0; i < uartndir; i++){
		if (strcmp(p, uartdir[i].name) == 0)
			break;
	}
	
	if (i==uartndir){
		SET_ERRNO(ENOENT);
		return -1;
	}
	// look for a file descriptor
	if ((fd = newfd()) == MAX_FILES){
		SET_ERRNO(ENFILE);
		return -1;
	}
	
	files[fd].type = devno (uartdevtab.dc);
	files[fd].qid = uartdir[i].qid;
	files[fd].pos = 0;
	return fd;
}

static long
uartread (int fd, void *str, long n, vlong offset) {
	int i=0, c;
	Qid qid;
	Uart *p;

	qid = files[fd].qid;
	if(qid.type & QTDIR){
		return -1;
	}

	p = &uart[NETID(qid.path)];
	switch(NETTYPE(qid.path)){
	case Ndataqid:
		while (i < n && (c=p->hw->ugetc(p->hw)) != '\0' && (c != 0x04)){ // EOT
			*((char*)str + i++)=c;
			DPRINT("uartread Ndataqid %c (%d)\n", c, c);
		}
		break;
	case Nctlqid:
		DPRINT("uartread Nctlqid\n");
		snprintf(str, n,
			"b%d b%d s%d p%c d%d\n",
			p->baud, p->bits, p->stop, p->parity, p->modem);
		break;
	case Nstatqid:
		DPRINT("uartread Nstatqid\n");
		snprintf(str, n,
			"uart port: %#X type: %s\n"
			"baud %d bits %d stop %d parity %c modem %d\n",
			p->hw->port , p->hw->name,
			p->baud, p->bits, p->stop, p->parity, p->modem);
		break;
	}
	
	return n;
}

static long
uartwrite (int fd, const void *str, long n, vlong offset) {
	int i=0;
	Qid qid;
	Uart *p;
	char *strc = (char *)str;

	qid = files[fd].qid;
	if(qid.type & QTDIR){
		return -1;
	}
	
	p = &uart[NETID(qid.path)];
	switch(NETTYPE(qid.path)){
	case Ndataqid:
		DPRINT("uartwrite Ndataqid\n");
		while (i<n){
			p->hw->uputc(p->hw, strc[i++]);
			DPRINT("uartwrite Ndataqid %c (%d)\n",strc[i-1], strc[i-1]);
		}
		break;
	case Nctlqid:
		DPRINT("uartwrite Nctlqid\n");
		p->uartctl(p, (char*)str);
		break;
	case Nstatqid:
		DPRINT("uartwrite Nstatqid\n");
		break;
	}

	return n;
}

Dev uartdevtab = {
	'u',
	"uart",
	
	uartinit,
	uartopen,
	uartread,
	uartwrite,
	devclose,
};

