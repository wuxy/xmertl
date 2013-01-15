#include <config.h>
#include <stdarg.h>
#include <string.h>

extern int __v_printf(struct arg_printf *fn, const char *format, va_list arg_ptr);

struct str_cdata {
  char* str;
  size_t len;
  size_t size;
};

static int swrite(void*ptr, size_t nmemb, struct str_cdata* sd) {
  size_t tmp=sd->size-sd->len;
  if (tmp>0) {
    size_t len=nmemb;
    if (len>tmp) len=tmp;
    if (sd->str) {
      memcpy(sd->str+sd->len,ptr,len);
      sd->str[sd->len+len]=0;
    }
    sd->len+=len;
  }
  return nmemb;
}

int vsnprintf(char* str, size_t size, const char *format, va_list arg_ptr) {
  int n;
  struct str_cdata sd = { str, 0, size?size-1:0 };
  struct arg_printf ap = { &sd, (int(*)(void*,size_t,void*)) swrite };
  n=__v_printf(&ap,format,arg_ptr);
  if (str && size && n>=0) {
    if (size!=(size_t)-1 && ((size_t)n>=size)) str[size-1]=0;
    else str[n]=0;
  }
  return n;
}

int vsprintf(char *dest, const char *format, va_list arg_ptr)
{
  return vsnprintf(dest,(size_t)-1,format,arg_ptr);
}

