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
#include "tracedump.h"


#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "xmbitopts.h"

struct rtl_trace *xm_trace;
struct rtl_trace_eventinfo_struct *rtl_trace_event_info = 0;
int fd0, fd1;

void do_rtl_dump(const char *stuff)
{
	printf(stuff);
}

static void cleanup(int signo)
{
	fprintf(stderr, "tracer cleanup (signal %d)\n", signo);
	close(fd0);
	close(fd1);
	exit(0);
}


int main()
{
	volatile unsigned long * trace_data_ready = NULL;
	volatile int *cpos;
	volatile int *ppos;

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

	printf("cpos %d ppos %d\n", *cpos, *ppos);

	/*
	 * open /dev/shm1, xmtrace puts a table of trace event id's
	 * and the corresponding event names there, so we can
	 * resolve the event names from the id's.
	 */
	if((fd1 = open("/dev/shm1", O_RDWR)) < 0) {
		printf("error opening device: /dev/shm1!\n");
		return -1;    
	}

	rtl_trace_event_info = mmap(NULL, sizeof(struct rtl_trace_eventinfo_struct) * RTL_TRACE_MAX_EVENTS, PROT_WRITE|PROT_READ, MAP_SHARED|MAP_FILE, fd1, 0);

	if (rtl_trace_event_info == NULL) {
		fprintf(stderr, "accessing shm1 failed\n");
		close (fd0);
		return -1;
	}

	while (1) {
		/* let the cpos pointer point to the start of the next buffer */
		*cpos = (*ppos + 1)%RTL_TNBUFFERS;
		/* test if there is new data avialable in the buffer */
		if (test_bit(*cpos, trace_data_ready)) {
			/* dump the data in the buffer onto stdout */
			printf("DUMP: cpos %d (data_ready) <%08lx> (overwritten) %lu finalize %lu\n", *cpos, *trace_data_ready, xm_trace->overwritten, xm_trace->finalize_count);
			rtl_trace_dump(&xm_trace->buffers[*cpos], &do_rtl_dump);
			fflush(stdout);
			clear_bit(*cpos, trace_data_ready);
			*ppos = *cpos;
		} 
		else {
			/* inform the user the currently tracing information has been found */
			printf("sorry no trace ready at cpos %d (data_ready) <%08lx> (overwritten) %lu finalize %lu\n", *cpos, *trace_data_ready, xm_trace->overwritten, xm_trace->finalize_count);
		}
		/* don't burn too much CPU time by polling too often */
		usleep(250000);
	}

/* never reached: */
	close (fd0);
	close (fd1);
	return 0;
}


