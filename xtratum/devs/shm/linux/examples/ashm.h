/*
 * $FILE: shm.h
 * 
 * Physical Memory Manager for SHM
 *
 * Conforming to ... i.e. POSIX 1003.13 
 */
/* Copyright (C) 2006,2007 DSLab Lanzhou University - 
 *           This code is Licensed under the terms of the GPL Version 2
 * Author    Bai Shuwei <baishw06@lzu.cn>
 *
 * Initial release 23/5/2007 
 *
 */

#ifndef __LINUX_SHM_H__
#define __LINUX_SHM_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <ctype.h>

#define DIGIT_LENGTH 12
#define LINUX_SHM_COUNT 16 


static inline int check_mbuff_name(const char *name)
{
	char MBUFF_NAME_HEAD[] = {"/dev/shm"};
	char minors[DIGIT_LENGTH] = {0};
	int i, j;

	if(strncmp(name, MBUFF_NAME_HEAD, strlen(MBUFF_NAME_HEAD)) != 0) {
		return -1;
	}

	for(j = 0, i = strlen(MBUFF_NAME_HEAD); isdigit(name[i]); j++, i++) {
		minors[j] = name[i];
	}

	minors[j] = '\0';
	j = atoi(minors);

	if(j < 0 || j >= LINUX_SHM_COUNT) {
		return -1;
	}

	return 0;
}

static inline char *mbuff_alloc(const char *name, ssize_t size)
{
	char *mbuf;
	int fd;

	if( check_mbuff_name(name) != 0) {
		return NULL;
	}

	if((fd = open(name, O_RDWR)) < 0) {
		return NULL;
	}

	mbuf = mmap(NULL, size,PROT_WRITE|PROT_READ,MAP_SHARED|MAP_FILE,fd, 0);
	if(mbuf == NULL) {
		close(fd);
		return NULL;
	}
	
	close(fd);

	if(mbuf < 0) mbuf = NULL;

	return mbuf;
}

static inline int mbuff_free(char *start, int size)
{
	return munmap(start, size);
}

#endif
