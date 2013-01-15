#include "LPC214x.h"
#include "system.h"
#include "uart.h"
#include "lib.h"

// BUG: why can't work with > 38400 ??
// the erase/programming flash loader neither
int
uartinit (unsigned long brate)
{
	unsigned long Fdiv;
	
	PINSEL0 = 0x00000005;				// Enable RxD0 and TxD0
	outl(Uart0+Lcr, Dlab|Wls8);	// 8 bits, no Parity, 1 Stop bit

	Fdiv = ( PCLK / 16 ) / brate ;     	// baud rate
	outl(Uart0+Dll, Fdiv % 256);	// set baud rate
	outl(Uart0+Dlm, Fdiv / 256);
	
	outl(Uart0+Lcr, Wls8);		// DLAB = 0

	outl(Uart0+Fcr, 0x07);		// enable FIFOs

	return 1;
}

/* Write character to Serial Port 0 with \n -> \r\n  */
int
putchar_serial0 (int ch){
	if (ch == '\n')  {
		while (!(inl(Uart0+Lsr) & Thre))
			;
		outl(Uart0+Thr, '\r');                  /* output CR */
	}
	while (!(inl(Uart0+Lsr) & Thre))
		;
	outl(Uart0+Thr, ch);
	return ch;
}

/* Write character to Serial Port 0 without \n -> \r\n  */
int
putc_serial0 (int ch){
	while (!(inl(Uart0+Lsr) & Thre))
		;
	outl(Uart0+Thr, ch);
	return ch;
}

void
putstring_serial0 (const char *string){
	char ch;

	while ((ch = *string)) {
		putchar_serial0(ch);
		string++;
	}
}

int
putstring_serialn (const char *string, int n){
	int i=0;

	while (i++<n)
		putchar_serial0(*(string++));
	return n;
}

/* read character from Serial Port   */
int
getkey_serial0 (void){
	if (inl(Uart0+Lsr) & Dr)
		return inl(Uart0+Rbr);
	return 0;
}

/* return a proper C ('\0' ended) string */
int
getstring_serialn (char *s, int n){
	int i, c;
	
	for (i=0; i < n; i++){
		while ( (c = getkey_serial0()) == 0)
			;
		s[i] = c;
	}
	s[i] = '\0';
	return n;
}
