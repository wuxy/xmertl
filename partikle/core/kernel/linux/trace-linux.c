#include <stdarg.h>
#include <arch/linux_syscalls.h>

static int trace_fd;		// trace file descriptor

int
trace_open (){
  trace_fd = linux_open ("sched_trace.ktr", O_WRONLY|O_CREAT|O_TRUNC, 00400|00200);
  return trace_fd;
}

/* This is a trick to print in a linux file */
extern int __v_printf(struct arg_printf *fn, const char *format, 
		      va_list arg_ptr);

static int ptrace_str (const char *str, int length, void *trace_fd) {
   return linux_write ((int) trace_fd, str, length);
}

// internal function to print trace
int print_trace (char *fmt, ...) {
  struct arg_printf _ap = { (void*)trace_fd, (int(*)(void*,size_t,void*)) ptrace_str};
  va_list vl;
  int res;
  
  va_start (vl, fmt);
  res = __v_printf (&_ap, fmt, vl);
  va_end (vl);
  return res;
}

