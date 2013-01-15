
typedef struct Uart Uart;
typedef struct HwUart HwUart;

struct Uart
{
	long	freq;			/* clock frequency */
	int	bits;			/* bits per character */
	int	stop;			/* stop bits */
	int	parity;			/* even, odd or no parity */
	int	baud;			/* baud rate */
	int	modem;			/* modem ctl */
	HwUart	* hw;
	int	console;		/* used as a serial console */
	int	special;		/* internal kernel device */
	
	int (*uartctl)(Uart *, char *);
	int (*uartstat)(Uart *, char *);
};

struct HwUart {
	char *name;
	int port;
	
	int (*pnp)(HwUart *u);	
	int (*baudr)(HwUart *, int);
	int (*modemctl)(HwUart *, int);
	int (*bits)(HwUart *, int);
	int (*stop)(HwUart *, int);
	int (*parity)(HwUart *, int);
	int (*fifoctl)(HwUart *, int);
	int (*interrupts)(HwUart *, int);
	
	int (*ugetc)(HwUart *);
	int (*uputc)(HwUart *, int);
};
