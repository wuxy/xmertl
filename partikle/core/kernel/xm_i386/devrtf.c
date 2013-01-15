/*
 * $FILE: devrtf.c
 *
 * fifo driver
 *
 * $VERSION: 1.0
 *
 * Author: Bai Shuwei <baishuwei@dslab.lzu.edu.cn> 
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
#include <fs.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <arch/xm_syscalls.h>

#include "rtf.h"

extern event_handling_t *event_handling;

static struct fifo_struct *sfifos[FIFO_COUNT];
static struct rt_fifo_struct fifos[FIFO_COUNT] = {{0, 0, 0, NULL}};  

extern struct File files [MAX_FILES];
extern int print_kernel (const char *str, unsigned long length);

static long rt_fifo_read(int index, void *dest, long size)
{
        struct fifo_struct *ff;
        unsigned long old_bom;
        unsigned long new_bom;
        unsigned long bom;
        long sz = size;

        if(INDEX_UNVALID(index) || ADDR_UNVALID(dest) || sz < 0) {
               	SET_ERRNO(ENODEV); 
                return 0;
        }

        ff = sfifos[index];

        if(ADDR_UNVALID(ff) || !FIFO_USABLE(ff) || !FIFO_ADDR(index)) {
		SET_ERRNO(ENODEV);
                return 0;
        }

/*
        if(FIFO_WRITING(ff)) {
                ff = &sfifos[(index + FIFO_COUNT) % FIFO_COUNT*2]; 
        }
*/

        SET_READ_FLAGS(ff);

        do {
                if(sz > FIFO_USED_SIZE(ff)) {
                       	sz = FIFO_USED_SIZE(ff); 
			if(!sz) return 0;
                }

                old_bom = FIFO_BOM(ff);
                new_bom = old_bom + sz;
                bom = old_bom % FIFO_SIZE(ff);

                if(FIFO_TOP(ff) % FIFO_SIZE(ff) >= bom) {
                        memcpy(dest, FIFO_ADDR(index) + bom, sz);
                } else {
                        if(sz >  FIFO_SIZE(ff) - bom) {
                                memcpy(dest, FIFO_ADDR(index) + bom,
                                        FIFO_SIZE(ff) - bom);
                                memcpy(dest + FIFO_SIZE(ff) -  bom, FIFO_ADDR(index),
                                        sz - (FIFO_SIZE(ff) - bom));
                        } else {
                                memcpy(dest, FIFO_ADDR(index) + bom, sz);
                        }
                }

        } while(CAS(&FIFO_BOM(ff), old_bom, new_bom) != old_bom);
        UNSET_READ_FLAGS(ff);

        if(FIFO_TOP(ff) == FIFO_BOM(ff)) {
                FIFO_TOP(ff) = 0;
                FIFO_BOM(ff) = 0;
	//	memset(fifos[index].rtf_addr, 0, 4096);
        }

        return sz;
}

static long rt_fifo_write(int index, const void *src, long size)
{
        long sz = size;
        long old_top;
        long new_top;
        long top;
        struct fifo_struct *ff;

        if(INDEX_UNVALID(index) || ADDR_UNVALID(src) || sz < 0) {
		SET_ERRNO(EINVAL);
                return 0;
        }
        ff = sfifos[index];
	
        if(ADDR_UNVALID(ff) || !FIFO_USABLE(ff) || !FIFO_ADDR(index)) {
		SET_ERRNO(ENODEV);
                return 0;
        }
/*
        if(FIFO_READING(ff)) {
                ff = &sfifos[(index + FIFO_COUNT) % (FIFO_COUNT*2)]; 
        }
*/

        SET_WRITE_FLAGS(ff);
        do {
                if(sz > FIFO_FREE_SIZE(ff)) {
                        sz = FIFO_FREE_SIZE(ff);
			if(!sz) return 0;
                }

                old_top = FIFO_TOP(ff);
                new_top = FIFO_TOP(ff) + sz;
                top = FIFO_TOP(ff) % FIFO_SIZE(ff);
		if(top <= FIFO_BOM(ff) % FIFO_SIZE(ff)) {
                        memcpy(FIFO_ADDR(index) + top, src, sz);
                } else {
                        if(FIFO_SIZE(ff) - top >= sz) {
                                memcpy(FIFO_ADDR(index) + top, src, sz);
                        } else {
                                memcpy(FIFO_ADDR(index) + top, src, \
                                        FIFO_SIZE(ff) - top);
                                memcpy(FIFO_ADDR(index), src + FIFO_SIZE(ff) - top, \
                                        sz - (FIFO_SIZE(ff) - top));
                        }
                }
        } while(CAS(&FIFO_TOP(ff), old_top, new_top) != old_top);

        UNSET_WRITE_FLAGS(ff);

        return sz;
}

static void *rt_fifo_flush(int index)
{
        struct fifo_struct *ff;

        if(INDEX_UNVALID(index)) {
		SET_ERRNO(EINVAL);
                return NULL;
        }

        ff = sfifos[index];

        FIFO_TOP(ff) = 0;
        FIFO_BOM(ff) = 0;
	memset(FIFO_ADDR(index), 0, FIFO_SIZE(ff));

        return FIFO_ADDR(index);
}

static int rt_fifo_ioctl(int index, int cmd, void *accept)
{
        void *ret;

        if(INDEX_UNVALID(index)) {
                return -1;
        }
        switch(cmd) {
                case FLUSH_FIFO:
                        if((ret = rt_fifo_flush(index)) == NULL) {
                                return -1;
                        } else return 0;
                default: return 0;
        }

        return 0;
}

/* */
static int rt_fifo_init(int findex)
{
	int ret = -1;
	

	if(findex < FIFO_COUNT) {
		sfifos[findex] = (struct fifo_struct *) (event_handling -> fctladdr + 
					findex * sizeof(struct fifo_struct));
		fifos[findex].rtf_addr = (char *)event_handling -> fdataddr[findex];  
		if(sfifos[findex] && fifos[findex].rtf_addr) {
			sfifos[findex]->ff_flags |= _FIFO_US;
			ret = 0;
		} 
	}
        return ret;
}

static void rt_fifo_exit(int findex)
{
	if(findex < FIFO_COUNT) {
                sfifos[findex] = 0;
                fifos[findex].rtf_addr = NULL;
        }
}

// rtf device driver

extern int newfd();
extern int freefd(int fd);

#define MINOR(fd) (files[fd].qid.path)

static long  
rtfread(int fd, void *dest, long size, long long offset) 
{
	int minor = MINOR(fd);
        int ret;

	xm_disable_events_flag();
        if(fifos[minor].rtf_flags == 0) {
		SET_ERRNO(ENODEV);
		xm_enable_events_flag();
		return 0;
        }

        if(dest == NULL || size < 0) {
		SET_ERRNO(EINVAL);
		xm_enable_events_flag();
		return 0;
        }
/*
        if(fifos[minor].rtf_flags & 0x1) {
                SET_ERRNO(EACCES);
                xm_enable_events_flag();
                return 0;
        }
*/
	ret = rt_fifo_read(minor, dest, size);
	xm_enable_events_flag();
        return ret;
}

static long 
rtfwrite(int fd, const void *src, long size, long long offset) 
{
	int minor = MINOR(fd);
	int ret;

	xm_disable_events_flag();
	if(fifos[minor].rtf_flags == 0) {
		SET_ERRNO(ENODEV);
		xm_enable_events_flag();
		return 0;
	}
	if(src == NULL || size < 0) {
		SET_ERRNO(EINVAL);
		xm_enable_events_flag();
		return 0;
        }
/*	if(!(fifos[minor].rtf_flags & 0x03)) {
		SET_ERRNO(EACCES);
                xm_enable_events_flag();
                return 0;
	}
*/
	ret = rt_fifo_write(minor, src, size);
	xm_enable_events_flag();
	return ret;
}

static int 
rtfioctl(int fd, unsigned int request, unsigned long para)
{
	return rt_fifo_ioctl(fd, request, (void *)para);
}

static int 
rtfmmap(int fd, void  *start,  int length, 
	     int prot, int flags, int offset, char **result)
{
	return 0;
}

static int 
rtfopen(char *name, int mode) 
{
	int minor;
	char *mn = name;
	char *pm;
	int fd = 0;

	while(*mn && !isdigit(*mn)) {
		mn++;
	}
	
	if(!(*mn)) {
	
		SET_ERRNO(ENOENT);
		return -1;
	}
	minor = strtoul(mn, &pm, 10);

	if(minor < 0 || minor >= FIFO_COUNT) {
		SET_ERRNO(EINVAL);	
		return -1;
	}
	xm_disable_events_flag();

	if ((fd = newfd()) >= MAX_FILES){
		xm_enable_events_flag();
        	freefd(fd);
        	SET_ERRNO(ENOMEM);
        	return -1;
  	}
	
	if(minor >= FIFO_COUNT) {
                xm_enable_events_flag();
		SET_ERRNO(ENODEV);
		return -1;
        }

	if(!fifos[minor].rtf_user++) {
		fifos[minor].rtf_flags |= 0x01000000; 
	}
	fifos[minor].rtf_flags |= mode;
	files[fd].qid.path = minor;
	files[fd].type = devno('f');
	xm_enable_events_flag();

	return fd;
}

static void 
rtfclose(int fd) 
{
	int minor = MINOR(fd);

	xm_disable_events_flag();

        if(minor < 0 || minor >= FIFO_COUNT) {
		xm_enable_events_flag();
		return;
	}

	if(!(--fifos[minor].rtf_user)) {
		fifos[minor].rtf_flags = 0;
	}

	xm_enable_events_flag();
}

int init_rtf(void)
{
        int index;

        for(index = 0; index < FIFO_COUNT; index ++) {
		if(rt_fifo_init(index) != 0)
			goto error;
        }

	return 0;	
error:
	while(--index) {
		rt_fifo_exit(index);
	}
        return -1;
}


//struct 
Dev rtfdevtab = {
	dc: 'f',
	name: "rtf",
	init: init_rtf,

	open: rtfopen,
	read: rtfread,
	write: rtfwrite,
	close: rtfclose,
	ioctl: rtfioctl,
	mmap: rtfmmap
};

