/*
 * $FILE: io.h
 *
 * Port's related functions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *
 * $LICENSE:  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#ifndef _IO_H_
#define _IO_H_

//#ifndef _KERNEL_
//#error Internal file. Do not include it in your sources.
//#endif

#define iodelay() \
  __asm__ __volatile__ ("pushl %eax; inb $0x80,%al; inb $0x80,%al; popl %eax")


/* Byte related in and out */
#define outb(val, port) \
  __asm__ __volatile__ ("outb %0, %%dx\n\t" \
                        :: "a" ((unsigned char) (val)), \
                        "d" ((unsigned short) (port)))

#define outb_p(val, port) ({ \
  outb (val, port); \
  iodelay (); \
})

#define inb(port) ({ \
  unsigned char __inb_port; \
  __asm__ __volatile__ ("inb %%dx, %0\n\t" : "=a" (__inb_port) \
                        : "d" ((unsigned short) (port))); \
  __inb_port; \
})

#define inb_p(port) ({ \
        unsigned char __inb_port; \
        __inb_port = inb (port); \
        iodelay (); \
        __inb_port; \
})

#define io_write(addr, val) \
  (*(volatile unsigned long *)(addr) = (val))

#define io_read(addr) \
  (*(volatile unsigned long *)(addr))

/* Short related ins and outs */

#define outw(val, port) \
  __asm__ __volatile__ ("outw %0, %%dx\n\t" \
                        :: "a" ((unsigned short) (val)), \
                        "d" ((unsigned short) (port)))

#define outw_p(val, port) ({ \
  outw (val, port); \
  iodelay (); \
})

#define inw(port) ({ \
  unsigned short __inw_port; \
  __asm__ __volatile__ ("inw %%dx, %0\n\t" : "=a" (__inw_port) \
                        : "d" ((unsigned short) (port))); \
  __inw_port; \
})

#define inw_p(port) ({ \
        unsigned short  __inw_port; \
        __inw_port = inw (port); \
        iodelay (); \
        __inw_port; \
})

/* Long related ins and outs */

#define outl(val, port) \
  __asm__ __volatile__ ("outl %0, %%dx\n\t" \
                        :: "a" ((unsigned long) (val)), \
                        "d" ((unsigned short) (port)))

#define outl_p(val, port) ({ \
  outw (val, port); \
  iodelay (); \
})

#define inl(port) ({ \
  unsigned long __inl_port; \
  __asm__ __volatile__ ("inl %%dx, %0\n\t" : "=a" (__inl_port) \
                        : "d" ((unsigned short) (port))); \
  __inl_port; \
})

#define inl_p(port) ({ \
        unsigned long  __inl_port; \
        __inl_port = inw (port); \
        iodelay (); \
        __inl_port; \
})

#ifdef _KERNEL_
extern int iopl_sys (int level);
#endif


#endif // _IO_H_
