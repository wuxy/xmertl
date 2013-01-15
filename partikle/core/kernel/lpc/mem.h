/*
 * PSR
 */
#define PsrMusr	0x10 	/* User Mode */
#define PsrMfiq	0x11	/* FIQ Mode */
#define PsrMirq	0x12	/* IRQ Mode */
#define PsrMsvc	0x13	/* Supervisor Mode */
#define PsrMabt	0x17	/* Abort Mode */
#define PsrMund	0x1B	/* Undefined Mode */
#define PsrMsys	0x1F		/* System Mode */
#define PsrMask	0x1F

#define PsrDfiq		0x00000040	/* disable FIQ interrupts */
#define PsrDirq		0x00000080	/* disable IRQ interrupts */

#define PsrV		0x10000000	/* overflow */
#define PsrC		0x20000000	/* carry/borrow/extend */
#define PsrZ		0x40000000	/* zero */
#define PsrN		0x80000000	/* negative/less than */

#define	KSTACK	256
#define	RAMZERO	0x40000000
#define	RAMSIZE	0x00008000
#define	RAMTOP	(RAMZERO+RAMSIZE)
/* note that ISP and IAP routines need 256 & 128 bytes of RAMTOP */


// Vectored Interrupt Controller (VIC)
#define	WDT_INT			0
#define	SWI_INT			1
#define	ARM_CORE0_INT	2
#define	ARM_CORE1_INT	3
#define	TIMER0_INT		4
#define	TIMER1_INT		5
#define	UART0_INT		6
#define	UART1_INT		7
#define	PWM0_INT		8
#define	I2C0_INT			9
#define	SPI0_INT			10
#define	SPI1_INT			11
#define	PLL_INT			12
#define	RTC_INT			13
#define	EINT0_INT		14
#define	EINT1_INT		15
#define	EINT2_INT		16
#define	EINT3_INT		17
#define	ADC0_INT		18
#define	I2C1_INT			19
#define	BOD_INT			20
#define	ADC1_INT		21
#define	USB_INT			22

#define	IRQ_SLOT_EN		0x20	/* bit 5 in Vector control register */
#define	VIC_SIZE			16

#define	VECT_ADDR_INDEX	0x100
#define	VECT_CNTL_INDEX 0x200

#define	VICBASE 0xFFFFF000
#define	VICVOFF 0x00000030
#define	VICDEFV 0x00000034
