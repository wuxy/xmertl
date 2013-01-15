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
#include <xmdev.h>
#include "shm.h"
#include <xmtrace.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thomas hisch");
MODULE_DESCRIPTION("xtratum Event Tracer (shm)");

#define SHM_SIZE (8*PAGE_SIZE)

//FIXME move struct def into header file
struct rtl_event_descriptor_struct {
	unsigned class;
};
	
extern struct rtl_event_descriptor_struct rtl_event_descriptor[];

/* a pointer to the trace event info list (in /dev/shm1) */
static struct rtl_trace_eventinfo_struct *rtl_trace_event_info;

/* used to initialize the part of xmtrace that is located in
 * the XtratuM core. The argument passed to it is a pointer to
 * the trace event buffer (/dev/shm0)
 */
extern int xmtrace_core_setup(void *);
/*
 * a function pointer, that is used to save the "NOP pointer" of rtl_trace.
 * it is used to restore the rtl_trace pointer when the module is unloaded.
 */
static void (*save_rtl_trace)(int event_id, long event_data, void * eip);
/*
 * The function in the core that acutally "does the tracing". This is the
 * function, that is called at a tracepoint. 
 */
extern void rtl_do_trace(int event_id, long event_data, void * eip);

/*
 *  rtl_trace_seteventcalls adds a new event class
 */
int rtl_trace_seteventclass (int event, unsigned classmask)
{
	if ((unsigned) event >= RTL_TRACE_MAX_EVENTS) {
		return -1;
	}

	rtl_event_descriptor[event].class = classmask;
	return 0;
}

/*
 *  rtl_trace_setevnetcalls adds a new trace event
 */
int rtl_trace_seteventname (int event, char *name)
{
	if ((unsigned) event >= RTL_TRACE_MAX_EVENTS) {
		return -1;
	}

	strncpy(rtl_trace_event_info[event].name, name, RTL_TRACER_NAMELEN - 1);
	rtl_trace_event_info[event].name[RTL_TRACER_NAMELEN - 1] = 0;
	return 0;
}

/*
 *  rtl_trace_init() initializes the statically defined trace events and trace classes,
 *  initiates the init function in the xtratum core (xmtrace_core_init) and 
 *  starts tracing, by setting the function pointer rtl_trace to rtl_do_trace().
 */

int rtl_trace_init (void *start)
{
	int i;

	/* 
	 * init statically defined trace classes. These include calsses for:
	 * interrupts, scheduler, and user.
         */

	for (i = 0; i < RTL_TRACE_MAX_EVENTS; i ++) {
		rtl_trace_seteventclass(i, RTL_TRACE_CLASS_DEFAULT);
	}
	for (i = RTL_TRACE_HARD_CLI; i < RTL_TRACE_SCHED_IN; i++) {
		rtl_trace_seteventclass(i, RTL_TRACE_CLASS_INTERRUPTS);
	}
	for (i = RTL_TRACE_SCHED_IN; i <= RTL_TRACE_SCHED_CTX_SWITCH; i++) {
		rtl_trace_seteventclass(i, RTL_TRACE_CLASS_SCHEDULER);
	}
	/* since there is no REDD in xtratum at the moment... */
	/*for (i = RTL_TRACE_REDD_RX; i <= RTL_TRACE_REDD_DEBUG; i++) {
		rtl_trace_seteventclass(i, RTL_TRACE_CLASS_REDD);
	}*/ 
	rtl_trace_seteventclass(RTL_TRACE_USER, RTL_TRACE_CLASS_USER);

	for (i = 0; i < RTL_TRACE_MAX_EVENTS; i ++) {
		rtl_trace_seteventname(i, "unknown");
	}

	rtl_trace_seteventname(RTL_TRACE_HARD_CLI, "hard cli");
	rtl_trace_seteventname(RTL_TRACE_HARD_STI, "hard sti");
	rtl_trace_seteventname(RTL_TRACE_HARD_SAVE_FLAGS, "hw_save_flags");
	rtl_trace_seteventname(RTL_TRACE_HARD_RESTORE_FLAGS, "hw_restore_flags");
	rtl_trace_seteventname(RTL_TRACE_HARD_SAVEF_AND_CLI, "rtl_no_interrupts");
	rtl_trace_seteventname(RTL_TRACE_LOCAL_INTERCEPT, "local_intercept entry");
	rtl_trace_seteventname(RTL_TRACE_LOCAL_INTERCEPT_EXIT, "local_intercept exit");
	rtl_trace_seteventname(RTL_TRACE_USER, "user");
	rtl_trace_seteventname(RTL_TRACE_USER2, "xm-domain");
	rtl_trace_seteventname(RTL_TRACE_SPIN_LOCK, "rtl_spin_lock");
	rtl_trace_seteventname(RTL_TRACE_SPIN_UNLOCK, "rtl_spin_unlock");
	rtl_trace_seteventname(RTL_TRACE_INTERCEPT, "rtl_intercept entry");
	rtl_trace_seteventname(RTL_TRACE_INTERCEPT_EXIT, "rtl_intercept exit");
	rtl_trace_seteventname(RTL_TRACE_SCHED_IN, "scheduler in");
	rtl_trace_seteventname(RTL_TRACE_SCHED_OUT, "scheduler out");
	rtl_trace_seteventname(RTL_TRACE_SCHED_CTX_SWITCH, "rtl_switch_to");
	/* since there is no REDD in xtratum at the moment...
	 * but REDD is on the ToDo list, therefore, just commenting it out. */
/*	rtl_trace_seteventname(RTL_TRACE_REDD_RX, "REDD RX-interrupt");
	rtl_trace_seteventname(RTL_TRACE_REDD_TX, "REDD TX-interrupt");
	rtl_trace_seteventname(RTL_TRACE_REDD_TX_ACK, "REDD TX-acknowledge");
	rtl_trace_seteventname(RTL_TRACE_REDD_DEBUG, "REDD debugging");
*/
	/* the finialze event is used to force xmtrace to dump the recently traced events */
	rtl_trace_seteventname(RTL_TRACE_FINALIZE, "trace finalize");

	/* init the part of xmtrace in the xtratum core */
	xmtrace_core_setup(start);

	/* save the rtl_trace function pointer, and let it point to rtl_do_trace */
	save_rtl_trace = rtl_trace;
	rtl_trace= rtl_do_trace;

	return 0;
}

void rtl_trace_destroy (void)
{
	/* set rtl_trace back to save_rtl_trace, to stop tracing. 
	   instead of rtl_do_trace, a NOP operation is performed now.
  	*/
	rtl_trace = save_rtl_trace;
}

#ifdef MODULE


int init_module(void)
{
	void *shm_ptr;
	int index;
	int err;
	void *tmp;
/* check if the size of shm0 and shm1 is big enough for our purposes, 
 * i.e. the trace buffer have to fit into shm0 and the trace event list
 * into shm1
 */
	if(SHM_SIZE < sizeof(struct rtl_trace) ||
			SHM_SIZE < sizeof(struct rtl_trace_eventinfo_struct) * RTL_TRACE_MAX_EVENTS)
	{
		printk("xmtrace: ERROR shm_size %d is too small - reduce RTL_TNBUFFERS or increase SHM_SIZE\n",
			sizeof(struct rtl_trace));
		return -1;
	}

/* open /dev/shm0, and get its address: */	
	index = xm_shm_open("shm0", 0UL, 0UL);
  
 	if (index < 0 ) {
  		printk("ERROR: shm_open failed!\n");
    	return -1;
  	}
	
	/* shm_ptr points to /dev/shm0 */
  	if((err = xm_shm_ioctl(index, XM_SHM_VADDR, (unsigned long *) &shm_ptr)) >= 0) {
  		 printk("dev shm0 init done.\n");
  	} else {
  		 printk("Error: init of shm0 failed!, errorcode: %d\n", err);
  	}

/* open /dev/shm1, and get its address: */	
	index = xm_shm_open("shm1", 0, 0);
  
 	if (index < 0) {
  		printk("ERROR: shm_open failed!\n");
    	return -1;
  	}	

	/* tmp points to /dev/shm1 */
  	if((err = xm_shm_ioctl(index, XM_SHM_VADDR, (unsigned long *) &tmp)) >= 0) {
  		 printk("dev shm1 init done.\n");
  	} else {
  		 printk("Error: init of shm1 failed!, errorcode: %d\n", err);
  	}

	/* if we sucessfully got the address of shm1, we use it as the rtl_trace_event_info buffer */
	/* i.e. it contains a list of all trace events (id, name, class of the events). 	   */	
	if(tmp)
		rtl_trace_event_info = (struct rtl_trace_eventinfo_struct *) tmp; 

	if (!rtl_trace_event_info) {
		printk("RTLinux Tracer: failed to allocate event info buffer\n");
		return -1;
	}

	/* shm0 is initialized in the core, the parameter is a pointer to the shm buffer */
	rtl_trace_init(shm_ptr);
 	printk("XtraTum Tracer initialized\n"); 

	return 0;
}

void cleanup_module(void)
{
	/* stop tracing */
	rtl_trace_destroy();
	/* destroy the buffers */
  	xm_shm_unlink(0);
  	xm_shm_unlink(1);
	printk("XtraTum Tracer removed\n");
}

#endif
