#include <config.h>
#include <stdarg.h>
#include <fs.h>

static int __fwrite(void*ptr, size_t nmemb, int fd) {
  return write_sys (fd, ptr, nmemb);
}

extern int __v_printf(struct arg_printf *fn, const char *format, va_list arg_ptr);

int vfprintf(int fd, const char *format, va_list arg_ptr)
{
  struct arg_printf ap = { (void *) fd, (int(*)(void*,size_t, void*)) __fwrite };
  return __v_printf(&ap, format, arg_ptr);
}
