#include <stdio.h>
#include <time.h>
#include <hwtimers.h>
#include <processor.h>

#include "system.h"
#include "mem.h"
#include "rtc.h"

enum
{
	RTCFREQ = 32768,
	PREINT_RTC = (PCLK/RTCFREQ) - 1,
	PREFRAC_RTC = PCLK - (PREINT_RTC+1) * RTCFREQ,
	SEC2USECS = 1000000L,
};

void
rtcgettime(RTCTime *time){
	time->usec = (CTC * SEC2USECS) / (RTCFREQ-1);
	time->sec = SEC;
	time->min = MIN;
	time->hour = HOUR;
	time->mday = DOM;
	time->wday = DOW;
	time->yday = DOY;
	time->mon = MONTH;
	time->year = YEAR;
}

void
rtcsettime(RTCTime *time){
	SEC = time->sec;
	MIN = time->min;
	HOUR = time->hour;
	DOM = time->mday;
	DOW = time->wday;
	DOY = time->yday;
	MONTH = time->mon;
	YEAR = time->year;
}

void
rtcsetalarm(RTCTime *alarm) {   
	ALSEC = alarm->sec;
	ALMIN = alarm->min;
	ALHOUR = alarm->hour;
	ALDOM = alarm->mday;
	ALDOW = alarm->wday;
	ALDOY = alarm->yday;
	ALMON = alarm->mon;
	ALYEAR = alarm->year;
}

void
rtcsetalarmmask(ulong amsk){
	/*--- Set alarm mask ---*/	  
	AMR = amsk;
}

enum
{
	EsaMin	= 60,
	EsaHour	= 60*EsaMin,
	EsaDay	= 24*EsaHour,
	EsaWeek	= 7*EsaDay,
};

static short dmsize[] = {
	365, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
static short ldmsize[] = {
	366, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// this should return (int *) [l]dmsize;
static short
yrsize(int yr, int i)
{
        if((yr % 4) == 0)
                return ldmsize[i];
        else
                return dmsize[i];
}

void
sec2rtc(ulong e, RTCTime *rtc){
        int d;
	long hms, day;

	hms = e % EsaDay;
	day = e / EsaDay;
	if (hms  < 0){
		hms += EsaDay;
		day -= 1;
	}

	// generate usec
	rtc->usec = (CTC * SEC2USECS) / (RTCFREQ-1);
		
	// generate hh:mm:ss
	rtc->sec = hms % 60;
	d = hms / 60;
	rtc->min = d % 60;
	d = d / 60 + 1;
	rtc->hour = d;

	// year number
	if(day >= 0)
		for(d = 1970; day >= yrsize(d,0); d++)
			day -= yrsize(d,0);
	else
		for (d = 1970; day < 0; d--)
			day += yrsize(d-1,0);
	rtc->year = d;
	
	// generate month
	for(d = 1; day >= yrsize(rtc->year, d); d++)
		day -= yrsize(rtc->year, d);
	rtc->mday = day + 1;
	rtc->mon = d;
}

ulong
rtc2sec(RTCTime *rtc){
	int i;
	ulong es;
	
	es = 0;
	
	// seconds per year
	for(i= 1970; i < rtc->year; i++){
		es += yrsize(i, 0) * EsaDay;
	}
	
	// seconds per month
	for(i=1; i < rtc->mon; i++){
		es += yrsize(rtc->year, i) * EsaDay;
	}
	
	es += (rtc->mday-1) * EsaDay;
	es += rtc->hour * EsaHour;
	es += rtc->min * EsaMin;
	es += rtc->sec;
	
	return  es;
}

RTCTime rtc;
volatile ulong rtcsecs;
volatile ulong alarm_on = 0;

void
rtcshowtime(int upd){
	RTCTime *t = &rtc;
	if (upd)
		sec2rtc(rtcsecs, t);

	printf("%d-%02d-%02d %02d:%02d:%02d %d us UTC\n",
		t->year, t->mday, t->mon,
		t->hour, t->min, t->sec, t->usec);
}

static void
rtcirqhand(void){
	ulong myILR = ILR;

	if (myILR & ILR_RTCALF){
		/* alarm interrupt */
		ILR |= ILR_RTCALF;	/* clear interrupt flag */
		alarm_on = 1;
	}
	
	if(0) printf("rtcirqhand %d\n", rtcsecs);
	if (myILR & ILR_RTCCIF) {
		/* counter increment interrupt */
		ILR |= ILR_RTCCIF;
		rtcsecs ++;
	}

	VICVectAddr = 0;		/* Acknowledge Interrupt */
}

static void rtcstart(void);

int
rtcinit(){
	RTCTime alarm;
	
	alarm_on = 0;
    
	AMR = 0;
	CIIR = 0;
	CCR = 0;

	PREINT = PREINT_RTC;
	PREFRAC = PREFRAC_RTC;
	
	rtcsecs = KERNDATE;
	sec2rtc(rtcsecs, &rtc);
	rtcsettime(&rtc);		/* Set local time */

	alarm = rtc;
	alarm.sec += 5;
	rtcsetalarm(&alarm);		/* set alarm time */
	
	// enable irq, to do time accounting
	if (install_irq(RTC_INT, (void *) rtcirqhand) == FALSE){
		return FALSE;
	}
	
	rtcstart();

	return 1;
}

static void
rtcstart(void){
	/* only "second match" will generate in alarm interrupt - all others masked */
	rtcsetalarmmask(/*AMRSEC|*/AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);

	/* Change in Second will generate an increment interrupt */
	CIIR = IMSEC;

	/* clear interrupt-flags alarm and increment */
	ILR |= (ILR_RTCCIF | ILR_RTCALF);

	/*--- Start RTC counters ---*/
	CCR |= CCR_CLKEN;
}

static void
rtcstop(void){
	/*--- Stop RTC counters ---*/
	CCR &= ~CCR_CLKEN;
}

// realtime & monotonic clocks

static hwtime_t
rtcread(void){
	return rtcsecs;
}

static duration_t
rtcread_nsec(void){
	hwtime_t secs = rtcsecs;
	hwtime_t ticks = CTC & 0x00007FFF;
	
	return (secs * NSECS_PER_SEC) + (ticks * NSECS_PER_SEC)/(RTCFREQ-1);
}

static hwtime_t
rtchwtime2dur(hwtime_t hw){
	hwtime_t secs = hw/RTCFREQ;
	hwtime_t ticks = hw%RTCFREQ;
	
	return (secs * NSECS_PER_SEC) + (ticks * NSECS_PER_SEC)/(RTCFREQ-1);
}

static duration_t
rtcgetres (void) {
	return rtchwtime2dur(RTCFREQ);
}

system_clock_t rtc_clock = {
	init_clock: (void*) rtcstart,
	gettime_hwt: rtcread,
	gettime_nsec: rtcread_nsec,
	hwtime2duration: rtchwtime2dur,
	getres: rtcgetres,
	shutdown_clock: rtcstop,
};
system_clock_t * realtime_clock = &rtc_clock;
system_clock_t * monotonic_clock = &rtc_clock;

void
rtclink(void){
	rtcinit();
}
