#include <xm_syscalls.h>

//int a[8192] = {0, 0};
#define COLUMNS                 80
/* The number of lines. */
#define LINES                   24
/* The attribute of an character. */
#define ATTRIBUTE               7
/* The video memory address. */
#define VIDEO                   0xB8000

/* Variables. */
/* Save the X position. */
static int xpos;
/* Save the Y position. */
static int ypos;
/* Point to the video memory. */
static volatile unsigned char *video;

/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
/*static void cls (void) {
  int i;
  
  video = (unsigned char *) VIDEO;
  
  for (i = 0; i < COLUMNS * LINES * 2; i++)
    *(video + i) = 0;
  
  xpos = 0;
  ypos = 0;
  }*/

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. */
static void itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  
  /* If %d is specified and D is minus, put `-' in the head. */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;
     
  /* Divide UD by DIVISOR until UD == 0. */
  do
    {
      int remainder = ud % divisor;
      
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);
  
  /* Terminate BUF. */
  *p = 0;
  
  /* Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}
     
/* Put the character C on the screen. */
static void xputchar (int c) {
  /*
  if (c == '\n' || c == '\r')
    {
    newline:
      xpos = 0;
      ypos++;
      if (ypos >= LINES)
	ypos = 0;
      return;
    }
  
  *(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
  *(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;
  
  xpos++;
  if (xpos >= COLUMNS)
    goto newline;
  */
  if (c != 0)
    write_scr (&c, 1);
}

/* Format a string and print it on the screen, just like the libc
   function printf. */
void xprintf (const char *format, ...)
{
  char **arg = (char **) &format;
  int c;
  char buf[20];
  
  arg++;
  
  while ((c = *format++) != 0)
    {
      if (c != '%')
	xputchar (c);
      else
	{
	  char *p;
	  
	  c = *format++;
	  switch (c)
	    {
	    case 'd':
	    case 'u':
	    case 'x':
	      itoa (buf, c, *((int *) arg++));
	      p = buf;
	      goto string;
	      break;
	      
	    case 's':
	      p = *arg++;
	      if (! p)
		p = "(null)";
	      
	    string:
	      while (*p)
		xputchar (*p++);
	      break;
	      
                 default:
                   xputchar (*((int *) arg++));
                   break;
	    }
	}
    }
}

void handler (int event, void *a) {
  //write_scr ("irq 0\n", 6);
  xprintf ("hola\n");
     //pass_event (event);
     //unmask_event (0);
}

int kmain (void) {
  struct xmitimerval req = {{0, 200000}, {0, 0}};
  //int n;

  //  a[8000]=10;
  install_event_handler (0, handler);
  
  set_timer (&req, 0);

  unmask_event (0);
  
  enable_events_flag();
  while (1)
    suspend_domain (0, 0);

  //return a [4];
  return 0;
}
