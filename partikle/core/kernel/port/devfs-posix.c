/*
 * $FILE: devfs-posix.c
 *
 * host filesystem driver
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
#include <fs.h>

static int
fsopen (char * name, int omode) {
  printf("fsopen (%s, %d)\n", name, omode);
  return 0;
}

static long
fsread (int fd, void *str, long n, vlong offset) {
  printf("fsread (fd, %X, %d, %d)\n", fd, str, n, offset);
  return 0;
}

static long
fswrite (int fd, const void *str, long n, vlong offset) {
  printf("fswrite (fd, %X, %d, %d)\n", fd, str, n, offset);
  return 0;
}

Dev fsdevtab = {
	'U',
	"fs",
	
	devinit,
	fsopen,
	fsread,
	fswrite,
	devclose,
};
