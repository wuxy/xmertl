#include <stdio.h>
#include <stdarg.h>
#include <fs.h>

static int trace_fd;		// trace file descriptor

int
trace_open (){
  trace_fd = open_sys ("/dev/rtf0", 0, 0);
  return trace_fd;
}

// internal function to print trace
int
print_trace (char *fmt, ...) {
  va_list vl;
  int res;
  
  va_start (vl, fmt);
  res = fprintf (trace_fd, fmt, vl);
  va_end (vl);
  return res;
}

