/*
 * $FILE: stdio.c
 *
 * Standard buffered input/output
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *
  * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <errstr.h>

static FILE stdvect[3] = {
	{.fd = STDIN_FILENO, .bufhead = 0, .buftail = 0, .bufsize = 0},
	{.fd = STDOUT_FILENO, .bufhead = 0, .buftail = 0, .bufsize = 0},
	{.fd = STDERR_FILENO, .bufhead = 0, .buftail = 0, .bufsize = 0},
};

FILE *stdin  = &stdvect[0];
FILE *stdout = &stdvect[1];
FILE *stderr = &stdvect[2];

//--------//
// printf //
//--------//

int printf(const char *format,...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vprintf(format, arg_ptr);
  va_end(arg_ptr);
  return n;
}

//------//
// putc //
//------//


/* TODO bug putc -> fputs (buffered io) */

inline int putc(int c, FILE *s) {
  if (write (STDOUT_FILENO, &c, 1) < 0)
	  return EOF;
  return c;
}

//---------//
// putchar //
//---------//

inline int putchar(int c) {
  return putc (c, stdout);
}

//--------//
// sscanf //
//--------//

int sscanf(const char *str, const char *format, ...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vsscanf(str,format,arg_ptr);
  va_end (arg_ptr);
  return n;
}

//-------//
// scanf //
//-------//

int scanf(const char *format, ...) {
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vfscanf(stdin,format,arg_ptr);
  va_end (arg_ptr);
  return n;
}

//---------//
// getchar //
//---------//
inline int getchar(void) {
	return fgetc(stdin);
}

//-------//
// fgetc //
//-------//

int fgetc(FILE *s) {
  int c;
  // filling the buffer
  if (s -> bufsize == 0) {
    s -> bufsize += read (s -> fd, s -> buffer, 1);
    s -> bufhead = 0;
    s -> buftail = s -> bufsize - 1;
  }

  s -> bufsize --;
  c = s -> buffer [s -> bufhead ++];
  if (s -> bufsize < 0)
	  return EOF;
  return c;
}

//------//
// getc //
//------//
inline int getc(FILE *s) {
	return fgetc(s);
}


//--------//
// ungetc //
//--------//

/*
 * TODO: Implement ungetc correctly
 */

int ungetc(int c, FILE *s) {
  if (c == EOF)
	  return EOF;
  if (s -> bufsize < FILE_BUFFER_SIZE) {
    s -> bufsize ++;
    s -> buffer [++s -> buftail] = c;
    return c;
  }
  return EOF;
}

//------//
// fputs //
//------//

int
fputs(const char *b, FILE *s)
{
  int n, ret;
  for (n = 0; b[n]; n++) {
    s -> bufsize ++;
    s -> buffer [++s -> buftail] = b[n];
  }

  if ((ret = write (s -> fd, &s -> buffer[s->bufsize-n], n+1)) < 0)
	  return EOF;
  s -> bufsize -= n;
  s -> buftail -= n;
  return ret;
}

//------//
// puts //
//------//

inline int
puts (const char *b)
{
  return fputs (b, stdout);
}

//---------//
// sprintf //
//---------//

int
sprintf (char *str, const char *format, ...)
{
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vsprintf(str, format, arg_ptr);
  va_end (arg_ptr);
  return n;
}

//----------//
// snprintf //
//----------//

int
snprintf (char *str, size_t size, const char *format, ...)
{
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vsnprintf(str, size, format, arg_ptr);
  va_end (arg_ptr);
  return n;
}

//---------//
// perror  //
//---------//

void
perror (const char* s)
{
  const char* msg = __sys_err_unknown;
  int err = errno;
  int n = sizeof (errstr) / sizeof (errstr[0]);

  if (n == 2)
    msg=errstr[1];
  else if (0 <= err && err < n)
     msg=errstr[err];

  if (s) {
    write (STDERR_FILENO, s, strlen(s));
    write (STDERR_FILENO, ": ", 2);
  }
  write (STDERR_FILENO, msg, strlen(msg));
  write (STDERR_FILENO, "\n", 1);
}


//--------//
// fgets  //
//--------//

char *
fgets(char *b, int n, FILE *s)
{
	int i, c;

	i=0;
	c=EOF;
	while (i < n-1){
		c=fgetc(s);
		if(c == EOF || c == '\n')
			break;
		b[i++]=c;
	}
	b[i]='\0';

	if(i==0 && c == EOF)
		return NULL;
	return b;
}

//---------//
// fprintf //
//---------//

int
fprintf (FILE *s, const char *format, ...)
{
  int n;
  va_list arg_ptr;
  va_start(arg_ptr, format);
  n=vfprintf(s->fd, format, arg_ptr);
  va_end (arg_ptr);

  // check n for errors
  return n;
}

