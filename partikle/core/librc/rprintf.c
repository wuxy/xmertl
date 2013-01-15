/*
 * rprintf.c
 * - printf() based on sprintf() from gcctest9.c Volker Oth
 * - Changes made by Holger Klabunde
 * - Changes made by Martin Thomas for the efsl debug output
 * - Changes made by speiro for lpc2000 devices for PaRTiKle
 */

#include <stdarg.h>
#include <string.h>

#define SCRATCH 20	//32Bits go up to 4GB + 1 Byte for \0

//Spare some program space by making a comment of all not used format flag lines
#define USE_LONG	// %lx, %Lu and so on, else only 16 bit integer is allowed
//#define USE_OCTAL // %o, %O Octal output. Who needs this ?
#define USE_STRING			// %s, %S Strings as parameters
#define USE_CHAR	// %c, %C Chars as parameters
#define USE_INTEGER // %i, %I Remove this format flag. %d, %D does the same
#define USE_HEX		// %x, %X Hexadezimal output
#define USE_UPPERHEX	// %x, %X outputs A,B,C... else a,b,c...
#ifndef USE_HEX
# undef USE_UPPERHEX		// ;)
#endif
#define USE_UPPER // uncommenting this removes %C,%D,%I,%O,%S,%U,%X and %L..
												// only lowercase format flags are used
#define PADDING					//SPACE and ZERO padding

typedef struct Fprint Fprint;
struct Fprint{
	int (*putc)(int c, void *a);
	void *a;
};

void
vrprintf(Fprint *fp, char const *fmt, va_list args)
{
	unsigned char scratch[SCRATCH];
	unsigned char fmt_flag;
	unsigned short base;
	unsigned char *ptr;
	unsigned char issigned=0;

#ifdef USE_LONG
// #warning "use long"
	unsigned char islong=0;
	unsigned char isvlong=0;
	unsigned long long u_val=0;
	long long s_val=0;
#else
	unsigned int u_val=0;
	int s_val=0;
#endif

	unsigned char fill;
	unsigned char width;

	for (;;){
		while ((fmt_flag = *(fmt++)) != '%'){			 // Until '%' or '\0' 
			if (!fmt_flag){ return; }
			fp->putc(fmt_flag, fp->a);
		}

		issigned=0; //default unsigned
		base = 10;

		fmt_flag = *fmt++; //get char after '%'

#ifdef PADDING
		width=0; //no formatting
		fill=0;	 //no formatting

		if(fmt_flag=='0' || fmt_flag==' ') //SPACE or ZERO padding	?
		 {
			fill=fmt_flag;
			fmt_flag = *fmt++; //get char after padding char
			while(fmt_flag>='0' && fmt_flag<='9')
			 {
				width = 10*width + (fmt_flag-'0');
				fmt_flag = *fmt++; //get char after width char
			 }
		 }
#endif

#ifdef USE_LONG
		islong=0; //default int value
		isvlong=0;
#ifdef USE_UPPER
		if(fmt_flag=='l' || fmt_flag=='L') //Long value 
#else
		if(fmt_flag=='l') //Long value 
#endif
		 {
			islong=1;
			fmt_flag = *fmt++; //get char after 'l' or 'L'
			if (fmt_flag=='l'){
				isvlong = 1;
				fmt_flag = *fmt++; //get char after 'l' or 'L'
			}
		 }
#endif

		switch (fmt_flag)
		{
#ifdef USE_CHAR
		case 'c':
#ifdef USE_UPPER
		case 'C':
#endif
			fmt_flag = va_arg(args, int);
			// no break -> run into default
#endif

		default:
			fp->putc(fmt_flag, fp->a);
			continue;

#ifdef USE_STRING
#ifdef USE_UPPER
		case 'S':
#endif
		case 's':
			ptr = (unsigned char*)va_arg(args, char *);
			while(*ptr) { fp->putc(*ptr, fp->a); ptr++; }
			continue;
#endif

#ifdef USE_OCTAL
		case 'o':
#ifdef USE_UPPER
		case 'O':
#endif
			base = 8;
			fp->putc('0', fp->a);
			goto CONVERSION_LOOP;
#endif

#ifdef USE_INTEGER //don't use %i, is same as %d
		case 'i':
#ifdef USE_UPPER
		case 'I':
#endif
#endif
		case 'd':
#ifdef USE_UPPER
		case 'D':
#endif
			issigned=1;
			// no break -> run into next case
		case 'u':
#ifdef USE_UPPER
		case 'U':
#endif

//don't insert some case below this if USE_HEX is undefined !
//or put			 goto CONVERSION_LOOP;	before next case.
#ifdef USE_HEX
			goto CONVERSION_LOOP;
		case 'x':
#ifdef USE_UPPER
		case 'X':
#endif
			base = 16;
#endif

		CONVERSION_LOOP:

			if(issigned) //Signed types
			 {
#ifdef USE_LONG
				if(isvlong) { s_val = va_arg(args, long long); }
				else if(islong) { s_val = va_arg(args, long); }
				else { s_val = va_arg(args, int); }
#else
				s_val = va_arg(args, int);
#endif

				if(s_val < 0) //Value negativ ?
				 {
					s_val = - s_val; //Make it positiv
					fp->putc('-', fp->a);		 //Output sign
				 }

				if(!isvlong)
					u_val = (unsigned long)s_val;
				else
					u_val = (unsigned long long)s_val;
			 }
			else //Unsigned types
			 {
#ifdef USE_LONG
				if(isvlong) {u_val = va_arg(args, unsigned long long); }
				else if(islong) { u_val = va_arg(args, unsigned long); }
				else { u_val = va_arg(args, unsigned int); }
#else
				u_val = va_arg(args, unsigned int);
#endif
			 }
		
			ptr = scratch + SCRATCH;
			*--ptr = 0;
			do
			 {
				char ch = u_val % base + '0';
#ifdef USE_HEX
				if (ch > '9')
				 {
					ch += 'a' - '9' - 1;
#ifdef USE_UPPERHEX
					ch-=0x20;
#endif
				 }
#endif					
				*--ptr = ch;
				u_val /= base;

#ifdef PADDING
				if(width) width--; //calculate number of padding chars
#endif
			} while (u_val);

#ifdef PADDING
		 while(width--) *--ptr = fill; //insert padding chars					
#endif

			while(*ptr) { fp->putc(*ptr, fp->a); ptr++; }
		}
	}
}


extern int putchar_serial0(int);
extern int print_kernel(char*, int);
int
putchar(int c) {
	print_kernel((char*)&c, 1);
	return c;
}

static int
printfputc(int c, void *a){
	unsigned long * nc = (unsigned long*) a;
	if(c == '\n'){
		putchar('\r');
		(*nc)++;
	}

	putchar(c);
	(*nc)++;
	return 1;
}

int
rprintf(char const *fmt, ...){
	int nc=0;
	Fprint fp = {printfputc, (void*)&nc};
	va_list args;
	
	va_start(args, fmt);
	vrprintf(&fp, fmt, args);
	va_end(args);
	return nc;
}

int
printf(char const *fmt, ...){
	int nc=0;
	Fprint fp = {printfputc, (void*)&nc};
	va_list args;
	
	va_start(args, fmt);
	vrprintf(&fp, fmt, args);
	va_end(args);
	return nc;
}

typedef struct Sdata Sdata;
struct Sdata{
	char *s;
	int *nc;
};

static int
sprintfputc(int c, void *a){
	Sdata *sd = (Sdata*) a;
	
	(*sd->s++) = c;
	(*sd->nc)++;
	return 1;
}

int
sprintf(char *s, char const *fmt, ...){
	int nc=0;
	Sdata sd = {s, &nc};
	Fprint fp = {sprintfputc, (void*)&sd};
	va_list args;
	
	va_start(args, fmt);
	vrprintf(&fp, fmt, args);
	va_end(args);
	return nc;
}

typedef struct Sndata Sndata;
struct Sndata {
	char *s;
	int *n;	// s size
	int *nc;
};

static int
snprintfputc(int c, void *a){
	Sndata *snd = (Sndata*) a;

	if (*snd->n > *snd->nc){
		if (snd->s) {
			snd->s[(*snd->nc)] = c;
			(*snd->nc)++;
			snd->s[(*snd->nc)]='\0';
		}
	}
	return 1;
}

int
snprintf(char *s, int n, const char *fmt, ...){
	int nc=0;
	Sndata snd = {s, &n, &nc};
	Fprint fp = {snprintfputc, (void*)&snd};
	va_list args;
	
	va_start(args, fmt);
	vrprintf(&fp, fmt, args);
	va_end(args);
	return nc;
}

extern int write_sys(int fd, const void *s, int n);

typedef struct Fdata Fdata;
struct Fdata {
	int *fd;
	int *nc;
};

static int
fprintfputc(int c, void *a){
  	Fdata *fd = (Fdata*) a;
	write_sys ((*fd->fd), &c, 1);
	(*fd->nc)++;
	return 1;
}

int
fprintf(int fd, const char *fmt, ...){
	int nc=0;
	Fdata	fdt = {&fd, &nc};
	Fprint fp = {fprintfputc, (void*)&fdt};
	va_list args;
	
	va_start(args, fmt);
	vrprintf(&fp, fmt, args);
	va_end(args);
	return nc;
}
