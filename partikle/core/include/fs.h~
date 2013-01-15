/*
 * $FILE: fs.h
 *
 * PaRTiKle's filesystem
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
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

#ifndef _FS_H_
#define _FS_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <stddef.h>
#include <linkage.h>
#include <dev.h>

enum {
	Nodev = -1,
	MAX_FILES = 128,	// Maximum number of files which can be opened at the same time
};


struct File {
  int type;			// device serving the file
  Qid qid;			// id of the file
  int flags;
  loff_t pos;
};

extern struct File files [MAX_FILES];
extern int newfd();
extern int freefd(int fd);

/*
struct file_operations {
  loff_t (*llseek) (struct File *, loff_t, int);
  ssize_t (*read) (struct File *, char *, size_t, loff_t *);
  ssize_t (*write) (struct File *, const char *, size_t, loff_t *);
  int (*ioctl) (struct File *, unsigned int, unsigned long);
  int (*mmap) (struct File *, void  *,  size_t, int, int, off_t, caddr_t *);
  int (*open) (struct File *);
  int (*release) (struct File *);
};
*/

extern asmlinkage int open_sys (const char *pathname, int flags, int mode);
extern asmlinkage int close_sys (int fd);
extern asmlinkage ssize_t write_sys (int fd, const void *buf, size_t count);
extern asmlinkage ssize_t read_sys (int fd, void *buf, size_t count);
extern asmlinkage caddr_t mmap_sys (void  *start, size_t length, 
				    int prot, int flags, int fd, off_t offset);
extern asmlinkage int munmap_sys (void *start, size_t length);
extern asmlinkage off_t lseek_sys (int fildes, off_t offset, int whence);
extern asmlinkage int ioctl_sys (int fd, unsigned cmd, unsigned long arg);

#endif
