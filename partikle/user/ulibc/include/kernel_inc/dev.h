/*
 * $FILE: dev.h
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

#include <stddef.h>

typedef struct Dev Dev;
typedef struct Dirtab Dirtab;
typedef struct Rootdata Rootdata;

typedef long long vlong;
typedef unsigned long ulong;
typedef unsigned long long uvlong;
typedef unsigned char uchar;

/* bits in Qid.type */
#define QTDIR           0x80            /* type bit for directories */
#define QTFILE          0x00            /* plain file */

/* bits in Dir.mode */
#define DMDIR           0x80000000      /* mode bit for directories */
#define DMREAD          0x4             /* mode bit for read permission */
#define DMWRITE         0x2             /* mode bit for write permission */

enum {
	KNAMELEN	= 28
};

struct Dev {
	int	dc;
	char*	name;

	int		(*init)(void);
	int		(*open)(char*, int);
	long	(*read)(int fd, void*, long, long long);
	long	(*write)(int fd, const void*, long, long long);
	void	(*close)(int fd);

	// wish we don't need it
	int 	(*ioctl) (int fd, unsigned, unsigned long);
	int		(*mmap) (int fd, void  *, int, int, int, int, char **);
};
	
typedef
struct Qid
{
	uvlong	path;
	ulong	vers;
	uchar	type;
} Qid;


struct Dirtab
{
	char	name[KNAMELEN];
	Qid	qid;
	long	length;
	long	perm;
};

struct Rootdata
{
	int	dotdot;
	void	*ptr;
	int	size;
	int	*sizep;
};

extern ulong ndevs;
extern Dev*	devtab[];

void mkqid(Qid *q, vlong path, ulong vers, int type);

// device management functions
int init_devs (void);
extern void links (void);
int devno (int dc);
int readstr(unsigned long off, char *buf, unsigned long n, char *str);

// generic device functions
int devinit (void);
int devopen (char* name, int omode);
long devread (int fd, void* str, long n, vlong offset);
long devwrite (int fd, void* str, long n, vlong offset);
void devclose (int fd);

