#include "LPC214x.h"
#include "system.h"
#include "mem.h"
#include "lib.h"

#include <time.h>
#include <hwtimers.h>
#include <processor.h>

#define	DPRINT if(0)rprintf

volatile uvlong t0counter = 0;
static timer_handler_t t0uhand = 0;

static void
t0inthand(void){
	t0counter ++;
	
	if(0)rprintf("t0inthand T0MR0 %d T0PR %d\n", T0MR0, T0PR);

	if (t0uhand){
		if(0)putstring_serial0("t0uhand\n");
		(*t0uhand) ();
	}
	
	T0IR |= 0x00000001;			/* clear interrupt flag */
	VICVectAddr = 0; // ack interrupt
}

void
enable_timer(char t){
	if (t == 0){
		T0TCR = 1;
	}
	else{
		T1TCR = 1;
	}
}

void
disable_timer(char t){
	if (t == 0){
		T0TCR = 0;
	}
	else{
		T1TCR = 0;
	}
}

void
reset_timer(char t){
	ulong reg;
	
	if (t == 0){
		reg = T0TCR;
		reg |= 0x02;
		T0TCR = reg;
	}
	else{
		reg = T1TCR;
		reg |= 0x02;
		T1TCR = reg;
	}
}

#define INTERVAL_1NS	((hwtime_t)PCLK/NSECS_PER_SEC) // - 1
int
timerinit(vlong nsperiod){
	t0counter = 0;
	T0MR0 = (nsperiod  * PCLK / NSECS_PER_SEC);
	T0MCR = 3;			/* Interrupt and Reset on MR0 */

	DPRINT("timerinit T0MR0 %d, %d\n", T0MR0, (nsperiod  * PCLK / NSECS_PER_SEC));
	if ( install_irq(TIMER0_INT, (void *) t0inthand) == FALSE )
		return FALSE;
	return TRUE;
}

// timers

static int
irq_timerinit(void){
	DPRINT("irq_timerinit\n");
	timerinit(1000000);
	return 0;
}

static void
irq_timerwrite(duration_t nsperiod){
	DPRINT("irq_timerwrite t0cnt %d (%lld, %016llx)\n", t0counter, nsperiod, nsperiod);

	disable_timer(0);
	T0MR0 =  ((ulong)(nsperiod  * PCLK/ NSECS_PER_SEC) - 1);
	T0MCR = 3;			/* Interrupt and Reset on MR0 */
	enable_timer(0);
}

static duration_t
irq_timergetmax(void){
	return (duration_t)((int)~0)*NSECS_PER_SEC/(duration_t)PCLK;
}

static duration_t
irq_timergetmin(void){
	return NSECS_PER_SEC/(duration_t)PCLK;
}

static void
irq_timerstop(void){
	DPRINT("irq_timerstop\n");
	disable_timer(0);
}

static timer_handler_t
irq_timerhandler(timer_handler_t nuh){
	timer_handler_t oh;
	
	DPRINT("irq_timerhandler %08x\n", nuh);
	
	oh = t0uhand;
	disableirq();
	t0uhand = nuh;
	enableirq();

	return oh;
}

hwtimer_t irq_timer = {
  init_hwtimer: irq_timerinit,
  set_hwtimer: irq_timerwrite,
  get_max_interval: irq_timergetmax,
  get_min_interval: irq_timergetmin,
  set_timer_handler: irq_timerhandler,
  shutdown_hwtimer: irq_timerstop,
};

	// monotonic clock
/*
	
static hwtime_t
timer0start(void){
	return 0;
}

static hwtime_t
timer0read(void){
	hwtime_t ticks=T0MR0;
	
	if (!ticks) // no match
		ticks = T0PR;
		
	DPRINT("timer0read %lld\n",
		(hwtime_t)t0counter * ticks / (hwtime_t)(PCLK));
	return (hwtime_t)t0counter * ticks / (hwtime_t)(PCLK);
}

static duration_t
timer0read_nsec(void){
	hwtime_t ticks=T0MR0;
	
	if (!ticks) // no match
		ticks = T0PR;
	
	DPRINT("timer0read_nsec %lld\n",
		(hwtime_t)t0counter * ticks * NSECS_PER_SEC / (hwtime_t)(PCLK));
	
	return ticks / (hwtime_t)(PCLK);
}

static hwtime_t
timer0hwtime2dur(hwtime_t hw){
	hwtime_t ticks=T0MR0;
	
	if (!ticks) // no match
		ticks = T0PR;
	
	DPRINT("timer0hwtime2dur %lld\n", (hwtime_t)(hw * PCLK) / ticks);
	return (hwtime_t)(hw * PCLK) / ticks;
}

static duration_t
timer0getres (void) {
	DPRINT("timer0getres %lld\n", timer0hwtime2dur(PCLK));
	return timer0hwtime2dur(PCLK);
}

static void
timer0stop(void){
	DPRINT("timer0stop\n");
}

system_clock_t timer0_clock = {
	init_clock: (void*) timer0start,
	gettime_hwt: timer0read,
	gettime_nsec: timer0read_nsec,
	hwtime2duration: timer0hwtime2dur,
	getres: timer0getres,
	shutdown_clock: timer0stop,
};
system_clock_t * monotonic_clock = &timer0_clock;
*/

// TODO add rtc also for port/time.c
hwtimer_t *hwtimers[1] = {
  &irq_timer,
};

int
getnrhwtimers (void) {
	return 1;
}

hwtimer_t*
get_best_hwtimer (void) {
	return &irq_timer;
}


void
timerslink(void){
	timerinit(1000000);
	enable_timer(0);
}
