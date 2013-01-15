/*
 * Released under the terms of GPL 2.
 */

/*
 * XMTRace initially based on the rtl_tracer module which was part of RTLinux/GPL
 * and originally written by Michael Barabanov at Finit State Machine Labs Inc.,
 * was reimplemented for the new XtratuM environment by Thomas Hisch 
 * (t.hisch@gmail.com) and Andreas Platschek (andi@opentech.at).
 */

#ifndef __RTL_TRACE_INT_H__
#define __RTL_TRACE_INT_H__

#define RTL_TRACE_MAX_EVENTS 64
#define RTL_TNBUFFERS 32
#define RTL_TNRECORDS 40

#define RTL_TRACER_FIFO 14


enum rtl_tracer_events_enum {
      	RTL_TRACE_HARD_CLI, RTL_TRACE_HARD_STI, RTL_TRACE_HARD_SAVE_FLAGS,
       	RTL_TRACE_HARD_RESTORE_FLAGS, RTL_TRACE_HARD_SAVEF_AND_CLI,

	RTL_TRACE_INTERCEPT, RTL_TRACE_INTERCEPT_EXIT,
       	RTL_TRACE_LOCAL_INTERCEPT, RTL_TRACE_LOCAL_INTERCEPT_EXIT,
	RTL_TRACE_SPIN_LOCK, RTL_TRACE_SPIN_UNLOCK, 

	RTL_TRACE_SCHED_IN, RTL_TRACE_SCHED_OUT, RTL_TRACE_SCHED_CTX_SWITCH,

	RTL_TRACE_REDD_RX, RTL_TRACE_REDD_TX, RTL_TRACE_REDD_TX_ACK,
	RTL_TRACE_REDD_DEBUG,

	RTL_TRACE_FINALIZE,
	RTL_TRACE_USER, RTL_TRACE_USER2 };

#define RTL_TRACE_CLASS_DEFAULT 	1
#define RTL_TRACE_CLASS_INTERRUPTS 	2
#define RTL_TRACE_CLASS_USER 		4
#define RTL_TRACE_CLASS_SCHEDULER 	8
#define RTL_TRACE_CLASS_REDD 		16


#define RTL_TRACER_NAMELEN 30
struct rtl_trace_eventinfo_struct {
	char name[RTL_TRACER_NAMELEN];
};

//#ifdef CONFIG_RTL_TRACER

extern void __hw_sti (void);
extern void __hw_cli (void);
extern int __hw_is_cli (void);
extern void __hw_save_flags (unsigned long *flags);
extern void __hw_save_flags_and_cli (unsigned long *flags);
extern void __hw_restore_flags (unsigned long flags);


extern int rtl_trace_seteventclass (int event, unsigned classmask);
extern int rtl_trace_seteventname (int event, char *name);

extern void (*rtl_trace)(int event_id, long event_data, void *eip);
extern void rtl_trace2(int event_id, long event_data);

struct rtl_trace_record {
	long long timestamp;
	long event_data;
	long eip;
	int event_id;
	int domain_id;
};

struct rtl_trace_buffer {
	int pos; /* index of the current writing position */
	int timestamp;
	int len;
	int id;
	unsigned long overwritten;
	struct rtl_trace_record trace[RTL_TNRECORDS];
};

struct rtl_trace {
	unsigned long trace_data_ready;
	unsigned long overwritten;
	unsigned long finalize_count;
	int reader_ppos, reader_cpos;
	int writer_cpos, writer_npos;
	struct rtl_trace_buffer buffers[RTL_TNBUFFERS];
};


/* #ifdef __KERNEL__ */

extern int rtl_trace_init (void *start); /* and the tracing begins */
extern unsigned rtl_trace_settracemask (unsigned mask);
extern void rtl_trace_destroy (void);

extern void set_tracer_flag(void);
extern void unset_tracer_flag(void);
/* #endif */

extern void rtl_trace_dump (struct rtl_trace_buffer *buffer, void (*dump)(const char *string));

#endif
