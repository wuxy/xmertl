/*
 * Released under the terms of GPL 2.
 */

/*
 * XMTRace initially based on the rtl_tracer module which was part of RTLinux/GPL
 * and originally written by Michael Barabanov at Finit State Machine Labs Inc.,
 * was reimplemented for the new XtratuM environment by Thomas Hisch 
 * (t.hisch@gmail.com) and Andreas Platschek (andi@opentech.at).
 */

/*
 * tracer.c contains the main part of the user space tool to read the 
 * tracers data from the shared memories.
 */

#include <stdio.h>
#include <signal.h>
#include <xmtrace.h>

#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "xmbitopts.h"

struct rtl_trace *xm_trace;
struct rtl_trace_eventinfo_struct *rtl_trace_event_info = 0;
int fd0;

static void cleanup(int signo)
{
	fprintf(stderr, "tracer cleanup (signal %d)\n", signo);
	close(fd0);
	exit(0);
}


int main()
{
	volatile unsigned long * trace_data_ready = NULL;
	volatile int *cpos;
	volatile int *ppos;
	int tracecnt=0;

	if (signal(SIGSEGV, SIG_IGN) != SIG_IGN) {
		signal(SIGSEGV, cleanup);
	}
	if (signal(SIGINT, SIG_IGN) != SIG_IGN) {
		signal(SIGINT, cleanup);
	}
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN) {
		signal(SIGTERM, cleanup);
	}
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN) {
		signal(SIGHUP, cleanup);
	}
	/*
	 * open /dev/shm0, xmtrace puts the trace events
	 * there, so we can access them.
	 */

	if((fd0 = open("/dev/shm0", O_RDWR)) < 0) {
		printf("error opening device: /dev/shm0!\n");
		return -1;    
	}


	xm_trace = mmap(NULL, sizeof(struct rtl_trace), PROT_WRITE|PROT_READ, MAP_SHARED|MAP_FILE, fd0, 0);
	if (xm_trace == NULL) {
		fprintf(stderr, "accessing shm0 failed\n");
		return -1;
	}

	trace_data_ready = &xm_trace->trace_data_ready;
	cpos = &xm_trace->reader_cpos;
	ppos = &xm_trace->reader_ppos;

	while (tracecnt<10) {
		/* let the cpos pointer point to the start of the next buffer */
		*cpos = (*ppos + 1)%RTL_TNBUFFERS;
		/* test if there is new data avialable in the buffer */
		if (test_bit(*cpos, trace_data_ready)) {
			/* dump the data in the buffer onto stdout */
			clear_bit(*cpos, trace_data_ready);
			*ppos = *cpos;
			tracecnt++;
		} 
		/* don't burn too much CPU time by polling too often */
		usleep(250000);
	}

	printf("Got my 10 traces!\n");
	close (fd0);
	return 0;
}


