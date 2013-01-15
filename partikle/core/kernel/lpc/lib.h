
typedef unsigned char	uchar;
typedef unsigned long	ulong;
typedef unsigned long long uvlong;
typedef long long	vlong;

#define nelem(v)	(sizeof(v) / sizeof(v[0]))

#define inb(a) (*(volatile unsigned char *)(a))
#define outb(a, v) (*(volatile unsigned char *)(a)) = (v)

#define inw(a) (*(volatile unsigned short *)(a))
#define outw(a, v) (*(volatile unsigned short *)(a)) = (v)

#define inl(a) (*(volatile unsigned long *)(a))
#define outl(a, v) (*(volatile unsigned long *)(a)) = (v)

int setr13(ulong m, ulong *s);

// lib.c
void delay(int i);
void rmemdmp(ulong* a, int nbytes, char* nfo);
void *rmemset(ulong*, int, int);
void *rmemcpy (ulong*, ulong*, int);
int rmemchk(ulong* , ulong* , int);

// rprintf.c
int rprintf(char const *format, ...);
int srprintf(char *s, char const *format, ...);

// trap.c
int trapinit(void);
void trap(void);
void dumpstate(void);
ulong getcpuid(unsigned reg);

ulong install_irq(ulong, void *);
ulong uninstall_irq(ulong);

ulong enableirq(void);
ulong disableirq(void);
ulong enablefiq(void);
ulong disablefiq(void);

//timers.c
int timerinit(vlong);
void enable_timer(char);
void disable_timer(char);

// rtc.c
int rtcinit(void);
//void rtcgettime(RTCTime *);
//void rtcsettime(RTCTime *);
//void rtcsetalarm(RTCTime *);
void rtcsetalarmmask(ulong);
void rtcshowtime(int upd);

// serial.c
int uartinit (ulong brate);
int putchar_serial0 (int ch);
int putc_serial0 (int ch);
void putstring_serial0 (const char *string);
int putstring_serialn (const char *string, int n);
int getkey_serial0 (void);
int getstring_serialn (char *string, int n);

// constants
#define	EOT	0x04
#define	TRUE	(1)
#define	FALSE	(0)
#ifndef NULL
#	define	NULL	((void *)0)
#endif
#define USED(arg, ...)	{ if (arg); if (__VA_ARGS__); }

// this doesn't belong here, but it's handy to share
extern ulong bbss[];
extern ulong ebss[];

extern ulong bdata[];
extern ulong edata[];

extern ulong btext[];
extern ulong etext[];

