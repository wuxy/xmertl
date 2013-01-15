/*
 * Released under the terms of GPL 2.
 */

/*
 * XMTRace initially based on the rtl_tracer module which was part of RTLinux/GPL
 * and originally written by Michael Barabanov at Finit State Machine Labs Inc.,
 * was reimplemented for the new XtratuM environment by Thomas Hisch 
 * (t.hisch@gmail.com) and Andreas Platschek (andi@opentech.at).
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <xmtrace.h>
#include <domain.h>
#include <linux/interrupt.h>

//FIXME move struct def into header file
struct rtl_event_descriptor_struct {
	unsigned class;
} rtl_event_descriptor[RTL_TRACE_MAX_EVENTS];
EXPORT_SYMBOL(rtl_event_descriptor);

static unsigned eventmask;
static int timestamp;

static struct rtl_trace *trace;
static struct rtl_trace_buffer *buffers;
static struct rtl_trace_buffer *cbuf;
#define CBUF (*cbuf)
static spinlock_t buffspin;

static int rtl_trace_finalize (int id);

static int len;
static struct rtl_trace_record *cur;
static struct rtl_trace_record *ebp;
static struct rtl_trace_record *sbp;

static int tracer_flag=0;

void set_tracer_flag(void) { tracer_flag = 1; }
void unset_tracer_flag(void) { tracer_flag = 0; }
int xm_in_interrupt(void) { return tracer_flag; }

#define LOCK_PREFIX ""
#define ADDR (*(volatile long *) addr)

static inline void xm_set_bit(int nr, volatile unsigned long *addr)
{
__asm__ __volatile__(LOCK_PREFIX
"btsl %1,%0"
:"=m" (ADDR)
:"Ir" (nr));
}

static inline void xm_clear_bit(int nr, volatile unsigned long *addr)
{
	__asm__ __volatile__( LOCK_PREFIX
		"btrl %1,%0"
		:"=m" (ADDR)
		:"Ir" (nr));
}

void rtl_do_trace(int event_id, long event_data, void *eip)
{
	unsigned long flags;

	if ((unsigned) event_id >= RTL_TRACE_MAX_EVENTS)
		return;
 	
	__hw_save_flags_and_cli(&flags);

	if (event_id == RTL_TRACE_FINALIZE) {
		rtl_trace_finalize(1);
		return;
	}

	if (rtl_event_descriptor[event_id].class & eventmask) {
		//xtratum does not support smp machines so spin lock here is useless
		//spin_lock (&buffspin);

		cur->timestamp =  sysclock.gettime_nsec ();
		cur->event_id = event_id;
		cur->eip = (long) eip;
		cur->event_data = event_data;
	
		if (xm_in_interrupt())
			cur->domain_id = -1;
		else	
			cur->domain_id = xm_current_domain->id;

		if (len < RTL_TNRECORDS)
			len++;
		if (++cur >= ebp)
			cur = sbp;
		
		//spin_unlock (&buffspin);
	}
	__hw_restore_flags (flags);
}
EXPORT_SYMBOL(rtl_do_trace);

static inline void init_current_buffer(void)
{
	struct rtl_trace_buffer *buff = &CBUF;
	len = 0;
	/* sbp = pointer to the start (first entry) of the buffer
	   ebp = pointer to the end (last entry) of the buffer
	   at init sbp == cur  (start trace == current trace)
	*/
	sbp = cur = &(buff->trace[0]);
	ebp = sbp + RTL_TNRECORDS;
}

/* the xmtrace_core_setup() function is called
   called from the xmtrace_shm.ko module 
   (loaded into the linux kernel) the parameter passed
   to it, is a pointer to the shm.
*/
int xmtrace_core_setup (void *xm_trace)
{
	if(!xm_trace)
		return -1;

	eventmask = 0xffffffff;
	timestamp = 0;
	spin_lock_init (&buffspin);

	/* let the trace pointer point to the shm buffer
	   and set all the buffer related stuff
	*/
	trace = (struct rtl_trace *)xm_trace;
	trace->trace_data_ready = 0;
	trace->trace_data_ready = 0;
	trace->reader_cpos = 0;
	trace->reader_ppos = -1;
	trace->writer_cpos = 0;
	trace->writer_npos = 0;
	trace->overwritten = 0;
	/* set the pointer to the buffers */
	buffers = trace->buffers;
	/* pointer to the current buffer (cbuf) */
	cbuf = &buffers[0];
	/* initialize the current buffer */
	init_current_buffer ();
	return 0;
}
EXPORT_SYMBOL(xmtrace_core_setup);

static int rtl_trace_finalize (int id)
{
	volatile int *npos, *cpos;
	
	//xtratum does not support smp machines at the moment, so spin lock here is useless (for now)
	//spin_lock (&buffspin); 

	++timestamp;
	cpos = &trace->writer_cpos;	
	npos = &trace->writer_npos;

	trace->finalize_count++;
	
	*npos = (*cpos + 1) % RTL_TNBUFFERS;
	/* signalize the userspace program, that new data is available,
	 * and ready to be fetched.
	 */
	xm_set_bit (*cpos, &trace->trace_data_ready);
	
	if(test_bit(*npos, &trace->trace_data_ready)) {
		trace->overwritten++; /* global overwritten counter */
		CBUF.overwritten++;  /* local overwritten counter */
		xm_clear_bit (*cpos, &trace->trace_data_ready);
		init_current_buffer (); /* start all over again */
	} else {
		CBUF.id = id;
		CBUF.timestamp = timestamp;
		CBUF.len = len;
		CBUF.pos = cur - sbp;

		cbuf = &trace->buffers[*npos];
		init_current_buffer ();
		*cpos = *npos;	
	}

	//spin_unlock (&buffspin);
	return 0;
}

