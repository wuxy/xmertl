/*
 * $FILE: paging.h
 *
 * Paging
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

#ifndef _PAGING_H_
#define _PAGING_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

#include <config.h>

#ifdef CONFIG_X86_PAGING
#define OFFSET 0xC0000000

#define PAGE_SHIFT 12
#define PGDIR_SHIFT 22

#define PAGE_SIZE 4096
#define PAGE_MASK (~(PAGE_SIZE - 1))

#define PAGE_DT_ENTRIES 1024
// APP PGD entries
#define APP_PGD_START 0
#define APP_PGD_END ((OFFSET - 1) >> PGDIR_SHIFT)

// PaRTiKle PGD entries
#define KERNEL_PGD_START (OFFSET >> PGDIR_SHIFT)
#define KERNEL_PGD_END (PAGE_DT_ENTRIES - 1)

// PAGE DIRECTORY/TABLE OPTIONS
#define _PG_PRESENT 0x001
#define _PG_RW 0x002
#define _PG_USER 0x004
#define _PG_PWT 0x008
#define _PG_PCD 0x010
#define _PG_ACCESSED 0x020
#define _PG_DIRTY 0x040
#define _PG_PSE 0x080
#define _PG_GLOBAL 0x100
#define _PG_UNUSED1 0x200
#define _PG_UNUSED2 0x400
#define _PG_UNUSED3 0x800

#define _PHY2VIRT(addr) (addr + OFFSET)
#define _VIRT2PHYS(addr) (addr - OFFSET)
#define PHYS2VIRT(addr) ((unsigned long) addr + OFFSET)
#define VIRT2PHYS(addr) ((unsigned long) addr - OFFSET)

#define ROUNDUP2PAGE(addr) \
  ((unsigned long) addr % PAGE_SIZE)? \
  ((unsigned long) addr & PAGE_MASK) + PAGE_SIZE : \
  (unsigned long) addr

#define PGTS_ENTRY (KERNEL_PGD_START + 1)
#define FREE_VMM_ENTRY (KERNEL_PGD_START + 2)

#else

#define _PHY2VIRT(addr) (addr)
#define _VIRT2PHYS(addr) (addr)
#define PHYS2VIRT(addr) ((unsigned long) addr)
#define VIRT2PHYS(addr) ((unsigned long) addr)
#define ROUNDUP2PAGE(addr) (addr)

#endif

#ifndef __ASSEMBLY__

#ifdef CONFIG_X86_PAGING
#define CLEAN_PAGE(addr) {\
  memset ((unsigned char *)addr, 0, PAGE_SIZE); \
}

#define load_pgd(pgd) {\
  __asm__ __volatile__ ("movl %0,%%cr3": :"r" (pgd)); \
}

#define save_pgd(pgd) {\
  __asm__ __volatile__ ("movl %%cr3, %0\n\t": "=r" (pgd) :); \
}

// virtual address to page directory entry
#define va2pgd(vaddress) ((vaddress) >> PGDIR_SHIFT)

// virtual address to page table entry
#define va2pgt(vaddress) (((vaddress) & 0x3FF000) >> PAGE_SHIFT)

// page directory and table to virtual address
#define pgdpgt2va(pgd, pgt) ((pgd << PGDIR_SHIFT) | (pgt << PAGE_SHIFT))

/*#define vm_map_page(pgd, paddr, vaddr, flags) { \
  unsigned long _pgd_e, _pgt_e, *_pgt; \
  \
  _pgd_e = va2pgd (vaddr); \
  _pgt_e = va2pgt (vaddr); \
  \
  _pgt = (unsigned long *)PHYS2VIRT((pgd [_pgd_e] & PAGE_MASK)); \
  _pgt [_pgt_e] = (paddr | flags); \
}

#define vm_unmap_page(pgd, vaddr) { \
  unsigned long pgd_e, pgt_e, *pgt; \
  \
  pgd_e = va2pgd (vaddr); \
  pgt_e = va2pgt (vaddr); \
  \
  pgt = (unsigned long *)PHYS2VIRT((pgd [pgd_e] & PAGE_MASK)); \
  pgt [pgt_e] = 0; \
  }*/

#define flush_tlb() { \
  unsigned int tmpreg; \
  __asm__ __volatile__("movl %%cr3, %0;  # flush TLB \n\t" \
                       "movl %0, %%cr3;\n\t" \
                       : "=r" (tmpreg) \
                       :: "memory"); \
}

#define flush_tlb_global() { \
  unsigned int cr4, cr4_a; \
  __asm__ __volatile__ ("movl %%cr4, %0\n\t" \
                        : "=r" (cr4)); \
  cr4_a = cr4 & (~CR4_PGE); \
  __asm__ __volatile__ ("movl %0,%%cr4\n\t" \
                        : : "r" (cr4_a)); \
  flush_tlb(); \
  __asm__ __volatile__ ("movl %0,%%cr4\n\t" \
                        : : "r" (cr4)); \
}

#define flush_tlb_all() { \
  if (cpu_has_pge()) { \
    flush_tlb_global(); \
  } else { \
    flush_tlb(); \
  } \
}

// pgd is the _ONLY_ page directory
extern unsigned int pgd [PAGE_DT_ENTRIES];

// the first 4 mbytes
extern unsigned int pg0 [PAGE_DT_ENTRIES];

// Here we map the pgts
extern unsigned int pg_pgt [PAGE_DT_ENTRIES];

#else

#define OFFSET 0

#endif


#endif // __ASSEMBLY__

#endif
