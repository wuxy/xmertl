/*
 * $FILE: devtrace.c
 *
 * Funtions to use the tracing (suitable for kiwi)
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
#include <processor.h>
#include <stdio.h>
#include <hwtimers.h>
#include <time.h>
#include <sched.h>
#include <syscalls.h>
#include <stdlib.h>
#include <trace.h>
#include <string.h>
#include <fs.h>

enum
{
	Qtrace,
};

static duration_t start_trace_time;
int last_trace_id = 1;		// last created pthread
int last_trace_mutex = 1;	// last created mutex
int last_trace_condv = 1;	// last created condition var
int last_trace_sem = 1;		// last created sem

static int traceenabled=0;	// trace disabled by default
int print_trace (char *fmt, ...);
int trace_open ();

static int
traceopen (char * name, int omode) {
	int fd;

	if (strncmp(name,"/dev/trace/ctl", 10)!=0){
		SET_ERRNO(EBADF);
		return -1;
	}
	if ((fd = newfd()) >= MAX_FILES){
		freefd(fd);
		SET_ERRNO(ENOMEM);
		return -1;
	}
	files[fd].type = devno('E');
	files[fd].qid = (Qid) {Qtrace};
	files[fd].pos = 0;
	return fd;
}

static long
traceread (int fd, void *str, long n, vlong offset) {
	struct File f = files[fd];

	switch(f.qid.path){
	case Qtrace:
		if (traceenabled)
			strncpy(str, "enable", 7);
		else
			strncpy(str, "disable", 8);
		return strlen(str);
	default:
		SET_ERRNO(ENOENT);
		return -1;
	}
}

static long
tracewrite (int fd, const void *str, long n, vlong offset) {
	struct File f = files[fd];

	switch(f.qid.path){
	default:
		SET_ERRNO(ENOENT);
		return -1;

	case Qtrace:
		if (strncmp(str, "enable", 6)==0)
			traceenabled=1;
		else if (strncmp(str, "disable", 7)==0)
			traceenabled=0;
		else{
			SET_ERRNO(ENOENT);
			return -1;
		}
		return strlen(str);
	}
}

#define MAGNITUDE 1000
#define TIME2QUIVI(t) ((long) ((float) (t - start_trace_time) / (float) MAGNITUDE ))

int log_event (struct Traceevent e) {
	char *color = "orange";
	struct timespec ts;

	if (!traceenabled)
		return 0;

	// timespec for longer trace sessions on kiwi
	ts = (struct timespec) {
		tv_sec:  (TIME2QUIVI(e.time)) / (long long) NSECS_PER_SEC,
		tv_nsec: (TIME2QUIVI(e.time)) % (long long) NSECS_PER_SEC
	};

	switch(e.etype){
	default:
		return -1;
	
	case start:
	case stop:
	case deadline:
	case execb:
	case exece:
	case readyb:
	case readye:
		print_trace("%d.%09ld %d %d\n",
			ts.tv_sec, ts.tv_nsec, e.etype, e.id);
		break;

	case lock:
	case unlock:
		// TODO use something like e.rtype = {mutex, condv, sem}
		// to handle un/locking over resources
		print_trace("%d.%09ld %d %d %d\n",
			ts.tv_sec, ts.tv_nsec, e.etype, e.id, e.res);
		break;

	case arrowup:
	case arrowdown:
		print_trace("%d.%09ld %d %d %s\n",
			ts.tv_sec, ts.tv_nsec, e.etype, e.id, e.text);
		break;

	case block:
		print_trace("%d.%09ld %d %d %s\n",
			ts.tv_sec, ts.tv_nsec, e.etype, e.id, e.text, color);
		break;

	case vline:
		print_trace("%d.%09ld %d %s\n",
			ts.tv_sec, ts.tv_nsec, e.etype, e.text);
		break;
	}
	return 0;
}

void syscall_pretrace (int syscall_nr){
	char lsys[4]; // 's' + syscall_nr + '\0'
	
	switch (syscall_nr){
	case hw_disable_irq_nr:
	case hw_enable_irq_nr:
	case hw_ack_irq_nr:
	case hw_end_irq_nr:
	case hw_cli_nr:
	case hw_sti_nr:
	case hw_restore_flags_nr:
	case hw_save_flags_nr:
	case hw_save_flags_and_cli_nr:
	case ufree_nr:
	case ualloc_nr: 
// don't log internal functions
		return;
	}

	snprintf(lsys, 4,"s%d", syscall_nr);
	log_event ((struct Traceevent) {
		etype: arrowdown,
		id: current_thread -> trace_id,
		time: monotonic_clock->gettime_nsec (),
		text: lsys
	});
}

void syscall_posttrace (int syscall_nr, int retval) {
	char lsys[4]; // 's' + syscall_nr + '\0'

	/* TODO: check return value of syscall,
	 * if it was successful perform tracing actions
	 */
	switch (syscall_nr){
	case hw_disable_irq_nr:
	case hw_enable_irq_nr:
	case hw_ack_irq_nr:
	case hw_end_irq_nr:
	case hw_cli_nr:
	case hw_sti_nr:
	case hw_restore_flags_nr:
	case hw_save_flags_nr:
	case hw_save_flags_and_cli_nr:
	case ufree_nr:
	case ualloc_nr:
// don't log internal functions
		return;

	case pthread_create_nr:
		//current_thread -> trace_id = last_trace_id ++;
		break;

	case sem_init_nr:
		//current_sem -> trace_id = last_trace_sem ++;
		break;

	case pthread_mutex_init_nr:
		//current_mutex -> trace_id = last_trace_mutex ++;
		break;

	case pthread_cond_init_nr:
		//current_condv -> trace_id = last_trace_condv ++;
		break;
	}
	//printf ("syscall: %d, retval: %d\n", syscall_nr, retval);

	snprintf(lsys, 4, "s%d", syscall_nr);
	log_event ((struct Traceevent) {
		etype: arrowup,
		id: current_thread -> trace_id,
		time: monotonic_clock->gettime_nsec (),
		text: lsys
	});
}

int
traceinit(void){
	if (trace_open ()<0){
		printf("%s: trace unable to init\n", __FILE__);
		return -1;
	}

	print_trace ("#kiwi default header\n"
		"PALETTE Rainbow\n"
		"DECIMAL_DIGITS 9\n"
		"DURATION 500\n"
		"PALETTE Rainbow\n"
		"ZOOM_X		 24\n"
		"ZOOM_Y		16\n"
		"COLOR EXEC-E 0 orchid4\n\n");

	start_trace_time = monotonic_clock->gettime_nsec ();
	return 0;
}

Dev tracedevtab = {
	'E',
	"trace",
	
	traceinit,
	traceopen,
	traceread,
	tracewrite,
	devclose,
};
