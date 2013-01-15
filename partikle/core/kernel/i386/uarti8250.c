#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <arch/io.h>

#include "uart.h"
#include "uarti8250.h"

/* UART type */
enum
{
	Uunknown,
	U16550a,
	U16450,
	U16550,
};
static char * uartname[] = {
	[Uunknown] = "Uunknown",
	[U16550a] = "U16550a",
	[U16550] = "U16550",
	[U16450] = "U16450",
};

// currently only perform uart type/name detection
// could perform minimal configuration
static int
i8250pnp(HwUart *hw){
	int type;
	char b;

	if (iopl_sys(3) == -1)
		return -1;
	b = inb(hw->port);
	outb(b|FIFOena, hw->port+Fcr);
	b = inb(hw->port+Iir);
	
	switch (b & Ifena){
	case 0x0:	// no FIFO	
		type = U16450;
		break;
	case 0x40:	// FIFO Enabled but unusable
		type = U16550;
		break;
	case 0xc0:	// FIFO Enabled
		type = U16550a;
		break;
	default:
		type = Uunknown;
		break;
	}
	hw->name = uartname[type];

	if (type == Uunknown)
		return -1;

	return 0;
}

static int
i8250baudr(HwUart *hw, int bauds){
	char b;
	int div;
	
	if (bauds > (UartFREQ/16))
		bauds = (UartFREQ/16);
	div = (UartFREQ/16) / bauds;
	b = inb(hw->port+Lcr);
	outb(b|Dlab, hw->port+Lcr);  /* enable DLAB */
	outb(div & 0x0000ff00, hw->port+Dlm);  /* Dlatch High */
	outb(div & 0x000000ff, hw->port+Dll);  /* Dlatch Low  */
	outb(b, hw->port+Lcr);		/* restore DLAB */ 
	
	if (0) printf("divisor: %d\n", 
		(0xff00 & inb(hw->port+Dlm)) | (0x00ff & inb(hw->port+Dll)));
	return bauds;
}

static int
i8250modemctl(HwUart *hw, int mode){
	unsigned int mcr;
	
	mcr = inb(hw->port+Mcr);
	mcr |= mode;
	outb(mcr, hw->port+Mcr);
	return mode;
}

static int
i8250bits(HwUart *hw, int wlen){
	int sctl;
	
	sctl = inb(hw->port+Lcr);
	switch(wlen){
	case 5:
		sctl |= Wls5;
		break;
	case 6:
		sctl |= Wls6;
		break;
	case 7:
		sctl |= Wls7;
		break;
	default:
	case 8:
		sctl |= Wls8;
		break;
	}
	outb(sctl, hw->port+Lcr);
	return wlen;
}

static int
i8250stop(HwUart *hw, int stopb){
	int sctl;
	
	sctl = inb(hw->port+Lcr);
	sctl &= ~Stp;	// sticky parity off
	switch(stopb){
	default:
	case 1:
		sctl &= ~Stb;
		break;
	case 2:
		sctl |= Stb;
		break;
	}
	outb(sctl, hw->port+Lcr);
	return stopb;
}

static int
i8250parity(HwUart *hw, int par){
	int sctl;
	
	sctl = inb(hw->port+Lcr);
	if (par == 'e')
		sctl |= (Pen|Eps);
	else if (par == 'o')
		sctl &= (Pen|~Eps);
	else
		sctl &= ~Pen;
	
	outb(sctl, hw->port+Lcr);
	return par;
}

static int
i8250fifoctl(HwUart *hw, int fena){
	int fctl;
	
	fctl = inb(hw->port+Fcr);
	if (fena)
		fctl |= (FIFO14|FIFOtclr|FIFOrclr|FIFOena);
	else
		fctl &= ~FIFOena;
	
	outb(fctl, hw->port+Fcr);
	return fena;
}

static int
i8250interrupts(HwUart *hw, int iena){
	outb(iena, hw->port+Ier);
	return iena;
}

static int
i8250ugetc(HwUart *hw){
	while ((inb(hw->port+Lsr) & Dr) == 0) // Data ready
		continue;
	return inb(hw->port+Rbr);
}

static int
i8250uputc(HwUart *hw, int c){
	while((inb(hw->port+Lsr) & Thre) == 0) // Thr empty
		continue;
	outb(c, hw->port+Thr);
	return c;
}

extern double log(double x);
extern float logf(float x);
extern long double logl(long double x);

#define logdec(x) ((x == 0)? 0: log(x)/log(10) + 1)
#define atoi(n) strtol(n, (char **) NULL, 10);
/*
 * where cstr can be like "bn ln pn sn fn dn"
 *  bn: n in [1, 115200] desired baud rate
 *  ln: n=5,6,7,8 word length in bits
 *  pn: n=o,e otherwise parity is disabled
 *  sn: n=1,2 number of stop bits
 *  fn: n=0,1 disable/enable fifo buffers
 *  dn: byte n, write n to Mcr register
 */

static int
uartctl(Uart *u, char *cstr){
	int clen;
	int i, fena;
	
	/* Turn off interrupts */
	u->hw->interrupts(u->hw, 0);
	/* Turn on DTR, RTS*/
	u->modem |= (Rts|Dtr);
	u->hw->modemctl(u->hw, u->modem);
	
	clen = strlen(cstr);
	for (i=0; i < clen-1; i++){
		if(0) printf("cstr[%d] = %c (%d)\n", i, cstr[i], cstr[i]);
		switch(cstr[i]){
		default:
			printf("error: unknown cmd: \"%s\" %c at i=%d\n", cstr, cstr[i], i);
			break;

		case ' ':
			break;

		case 'B':
		case 'b':
			u->baud = atoi(cstr+i+1);
			if (u->hw->baudr(u->hw, u->baud) != u->baud)
				printf("error: could not set uart: \"%c%d\"", cstr[i], u->baud);
			i += (int) logdec(u->baud);
			break;
		
		case 'D':
		case 'd':
			u->modem = atoi(cstr+i+1);
			if (u->hw->modemctl(u->hw, u->modem) != u->modem)
				printf("error: could not set uart: \"%c%d\"", cstr[i], u->modem);
			i += (int) logdec(u->modem);
			break;
		
		case 'L':
		case 'l':
			u->bits = atoi(cstr+i+1);
			if (u->hw->bits(u->hw, u->bits) != u->bits)
				printf("error: could not set uart: \"%c%d\"", cstr[i], u->bits);
			i += (int) logdec(u->bits);
			break;
			
		case 'S':
		case 's':
			u->stop = atoi(cstr+i+1);
			if (u->hw->stop(u->hw, u->stop) != u->stop)
				printf("error: could not set uart: \"%c%d\"", cstr[i], u->stop);
			i += (int) logdec(u->stop);
			break;
	
		case 'P':
		case 'p':
			u->parity = cstr[i+1];
			if (u->hw->parity(u->hw, u->parity) != u->parity)
				printf("error: could not set uart: \"%c%c\"", cstr[i], u->parity);
			i++;
			break;
		
		case 'f':
		case 'F':
			fena = cstr[i+1];
			if (u->hw->fifoctl(u->hw, fena) != fena)
				printf("error: could not set uart: \"%c%d\"\n",cstr[i], fena);
			i++;
			break;
		}
	}

	return 0;
}

/*
static void
debuguart(HwUart *hw){
	unsigned char lsr, mcr, msr;
	lsr = inb(hw->port+Lsr);
	mcr = inb(hw->port+Mcr);
	msr = inb(hw->port+Msr);

	printf(
		"Lsr oe %#X pe %#X fe %#X FIFOe %#X\n"
		"Mcr dtr %#X rts %#X Ie %#X\n"
		"Msr cts %#X dsr %#X ri %#X dcd %#X\n",
		lsr&Oe, lsr&Pe, lsr&Fe, lsr&FIFOerr,
		mcr&Dtr, mcr&Rts, mcr&Ie,
		msr&Cts, msr&Dsr, msr&Ri, msr&Dcd);
}
*/

static HwUart i8250hw0 = {
	.name = "",
	.port = Uart0,
	
	.pnp = i8250pnp,
	.baudr = i8250baudr,
	.modemctl = i8250modemctl,
	.bits = i8250bits,
	.stop = i8250stop,
	.parity = i8250parity,
	.fifoctl = i8250fifoctl,
	.interrupts = i8250interrupts,

	.ugetc = i8250ugetc,
	.uputc = i8250uputc,
};

static HwUart i8250hw1 = {
	.name = "",
	.port = Uart1,
	
	.pnp = i8250pnp,
	.baudr = i8250baudr,
	.modemctl = i8250modemctl,
	.bits = i8250bits,
	.stop = i8250stop,
	.parity = i8250parity,
	.fifoctl = i8250fifoctl,
	.interrupts = i8250interrupts,

	.ugetc = i8250ugetc,
	.uputc = i8250uputc,
};

Uart uart[] = {
	{
		.hw = &i8250hw0,
	
		.uartctl = uartctl,
		.uartstat = NULL,
	},
	{
		.hw = &i8250hw1,
	
		.uartctl = uartctl,
		.uartstat = NULL,
	},

	{(int) NULL},
};

