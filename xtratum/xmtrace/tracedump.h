/*
 * Released under the terms of GPL 2.
 */

/*
 * XMTRace initially based on the rtl_tracer module which was part of RTLinux/GPL
 * and originally written by Michael Barabanov at Finit State Machine Labs Inc.,
 * was reimplemented for the new XtratuM environment by Thomas Hisch 
 * (t.hisch@gmail.com) and Andreas Platschek (andi@opentech.at).
 */

#ifndef __RTL_TRACEDUMP_H__
#define __RTL_TRACEDUMP_H__

/* this is user-space code */
#include <xmtrace.h>

extern struct rtl_trace_eventinfo_struct *rtl_trace_event_info;

const char *eventname(int event_id)
{
	if ((unsigned) event_id >= RTL_TRACE_MAX_EVENTS) {
		return "unknown";
	}

	return rtl_trace_event_info[event_id].name;
}

void rtl_trace_dump (struct rtl_trace_buffer *buf, void (*do_dump)(const char *string))
{
	int s;
	int i;
	char sbuf[80];
	long long prevts = 0;
	long diff = 0;

	if (buf->len == 0) {
		do_dump("trace buffer empty\n");
	}
	s = buf->pos - buf->len;
	if (s < 0) {
		s += RTL_TNRECORDS;
	}
	for (i = 0; i < buf->len; i++) {
		diff = buf->trace[s].timestamp - prevts;
		prevts = buf->trace[s].timestamp;
		sprintf(sbuf, "D%2d %6ld %-23s %#10lx <0x%08lx>\n",
			buf->trace[s].domain_id,
			diff,
			eventname(buf->trace[s].event_id),
			buf->trace[s].event_data,
			buf->trace[s].eip
				/*,
			(unsigned) (buf->trace[s].timestamp >> 32),
			(unsigned) (buf->trace[s].timestamp & 0xffffffff) */
			);
		do_dump(sbuf);
		s++;
		if (s >= RTL_TNRECORDS) {
			s = 0;
		}
	}
	sprintf(sbuf, "That was trace # %d (lost %lu traces befor this trace)\n", buf->timestamp, buf->overwritten);
	buf->overwritten = 0; //buffer was successfully read so set overwritten back to 0 
	do_dump(sbuf);
}

#endif
