

#ifndef _XM_BITMAP_H_
#define _XM_BITMAP_H_

#ifndef UXMLIB
#include <config.h>
//#include <arch/XtratuM_asm.h>
//#include <linux/string.h>
#else
#include <xm/config.h>
#endif


#if MAX_EVENTS > 32 
#error MAX_EVENTS CAN NOT BE GREATER THAN 32 due to efficience reasons
#endif

#define BITMAP_SIZE 1

typedef unsigned long bitmap_t;

// Remember to make all these operations atomically

#define set_bit(bitmap, pos) bitmap |= (1 << pos)

#define clear_bit(bitmap, pos) bitmap &= ~(1 << pos)

#define is_bit_set(bitmap, pos) (bitmap & (1 << pos))

#define clear_bitmap(bitmap) bitmap = 0

#define set_bitmap(bitmap) bitmap = (~0)

#define is_some_bit_set(bitmap) bitmap

#define get_domain_pending_events(pending, mask) (~mask & pending)

static inline int _ffs(unsigned long x) {
  int r;
  
  __asm__("bsfl %1,%0\n\t"
	  "jnz 1f\n\t"
	  "movl $-1,%0\n"
	  "1:" : "=r" (r) : "rm" (x));
  return r;
}

#define get_next_set_bit_and_clear(_pending, _mask) ({ \
  unsigned int _pos = _ffs(get_domain_pending_events (_pending, _mask)); \
  if (_pos != -1) { \
    clear_bit (_pending, _pos); \
  } \
  _pos; \
})

#endif
