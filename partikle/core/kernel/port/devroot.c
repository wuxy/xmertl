/*
 * $FILE: devroot.c
 *
 * Read only root filesystem
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
#include <fs.h>
#include <stdio.h>
#include <string.h>

extern Rootdata rootdata[];
extern Dirtab roottab[];
extern int	rootmaxq;

/* TODO: use skipslash instead
 * tokenize a path filling an array of names between slashes
 * and return the number of names in the path.
 * warning: it modifies the path provided replacing '/' by '\0'
 */
static int
slice (char * name[], char path[]){
	int i,j;
	
	i=j=0;
	if (path[0] != '/')
		name[j++] = &path[i];
	
	while (1) {
		if (path[i] == '/'){
			path[i] = '\0';
			name[j++] = &path[i+1];
		}
		else if (path[i] == '\0'){
			name[j++] = &path[i+1];
			break;
		}
		i++;
	}
	
	return j-1;
}

enum {
	MAXDEPTH=10, // max fs tree depth
	MAXPATH=64,  // max size of a fs path
};

static int
rootopen (char * name, int omode) {
	int fd, dotdot=0;
	int i, ntab, j=0;
	char namecopy[MAXPATH];
	char * tabname[MAXDEPTH] = {0};
	
	strncpy(namecopy, name, MAXPATH);
	ntab = slice(tabname, namecopy);

	// walk filesystem to reach name
	for (i=0; i < ntab; i++) {
		for (j=0; j < rootmaxq && strcmp(tabname[i], roottab[j].name); j++)
			if (roottab[j].qid.type & QTDIR)
				dotdot=j;

		if (j == rootmaxq) {
			SET_ERRNO(ENOENT);
			return -1;
		}
		if (roottab[j].qid.type & QTFILE)
					break; // found
	}

	if (rootdata[j].dotdot != dotdot){
		SET_ERRNO(ENOENT);
		return -1;
	}
	
	// look for a file descriptor
	if ((fd = newfd()) == MAX_FILES){
		SET_ERRNO(ENFILE);
		return -1;
	}

	files[fd].type = devno ('/');
	files[fd].qid = roottab[j].qid;
	files[fd].pos = 0;
	return fd;
}

static long
rootread (int fd, void *str, long n, vlong offset) {
	struct Qid fq;
	unsigned long len;
	void * data;
	
	fq = files[fd].qid;
	len = rootdata[fq.path].size;
	if (len == 0 && rootdata[fq.path].sizep)
		len = *(rootdata[fq.path].sizep);

	if (offset < 0 || offset >= len)
		return 0;
	if (offset+n > len)
		n = len - offset;
	data = rootdata[fq.path].ptr;

	if (memmove (str, data+offset, n))
		files[fd].pos += n;
	return n;
}

static long
rootwrite (int fd, const void *str, long n, vlong offset) {
	SET_ERRNO(EPERM);
	return -1;
}

Dev rootdevtab = {
	'/',
	"root",
	
	devinit,
	rootopen,
	rootread,
	rootwrite,
	devclose,
};
