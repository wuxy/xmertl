/*
 * $FILE: mm.h
 *
 * XM's memory manager
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

#ifndef _XM_ARCH_MM_H_
#define _XM_ARCH_MM_H_

#include <arch/cpu.h>

// Definition of the MM flags (8 bits)
#define PAGE_WITHOUT_USE 0x0 // Not in use
#define PAGE_T 0x1 // Allocated to the page table
#define PAGE_PD 0x1 // Used as PD
#define PAGE_PT 0x2 // Used as PT

#define roundup2page(addr) \
  ((unsigned long) addr % PAGE_SIZE)? \
  ((unsigned long) addr & PAGE_MASK) + PAGE_SIZE : \
  (unsigned long) addr

#define size2npages(size) \
  ((size % PAGE_SIZE)?size/PAGE_SIZE + 1:size/PAGE_SIZE)

// Virtual address to page directory entry
#define va2pd(vaddress) (vaddress >> PGDIR_SHIFT)

// Virtual address to page table entry
#define va2pt(vaddress) ((vaddress & 0x3FF000) >> PAGE_SHIFT)

// Page directory and page table to virtual address
#define pdpt2va(pd, pt) ((pd << PGDIR_SHIFT) | (pt << PAGE_SHIFT))

// Next macro allows to obtain a pt address through the page directory
#define get_pd_addr(pd, pd_entry) \
  ((unsigned long) __va (((unsigned long *)pd) [pd_entry] & PAGE_MASK))

// And the following one allows to  obtain a page address via the page
// table
#define get_pt_addr(pt, pt_entry) \
  ((unsigned long)__va (((unsigned long *)pt) [pt_entry] & PAGE_MASK))

static inline unsigned long virt2phys_pd (unsigned virt_address) {
  unsigned long pd, pt, pd_entry, pt_entry, phys_addr;
  save_pd (pd);

  pd_entry = va2pd (virt_address);
  pt_entry = va2pt (virt_address);

  if ((((unsigned long *)pd) [pd_entry] & _PAGE_PSE)) {
    phys_addr = (((unsigned long *)pd) [pd_entry] & PAGE_MASK);
  } else {
    pt = get_pd_addr (pd, pd_entry);
    phys_addr = (((unsigned long *)pt) [pt_entry] & PAGE_MASK);
  }
  return phys_addr;
}

static inline void fill_pd_entry (unsigned long pd,
                                     unsigned long pd_entry,
                                     unsigned long pt,
                                     unsigned long flags) {
  ((unsigned long *)pd) [pd_entry] =
    ((__pa (pt) & PAGE_MASK) | (flags & 0xFFF));
}

static inline void fill_pt_entry (unsigned long pt,
                                     unsigned long pt_entry,
                                     unsigned long page,
                                     unsigned long flags) {
  ((unsigned long *)pt) [pt_entry] =
    ((__pa (page) & PAGE_MASK) | (flags & 0xFFF));
}

static inline unsigned long create_page_directory (unsigned long 
						   (*alloc_page) (void)) {
  unsigned long pd = (*alloc_page) (), c_pd;

  if (!pd) return pd;
  save_pd(c_pd);

  memset  ((unsigned char *) &((unsigned long *) pd)[0],
           0, 1024 * sizeof (unsigned long));

  memcpy ((unsigned char *) &((unsigned long *) pd)[va2pd(PAGE_OFFSET)],
          (unsigned char *) &((unsigned long *) c_pd)[va2pd(PAGE_OFFSET)],
          (1024 - va2pd(PAGE_OFFSET)) * sizeof (unsigned long));

  return pd;
}

// This functions is useful for allocate a single page in the user map
static inline unsigned long allocate_user_page (unsigned long pd,
						unsigned long vaddress, 
						unsigned long (*alloc_page)
						(void)) {
  unsigned long pt_entry = va2pt(vaddress),
    pd_entry = va2pd(vaddress), pt, page = 0;
  
  if (vaddress >= PAGE_OFFSET) return 0;
  
  // Check if there is already an allocated pt in the pd table
  if (!(((unsigned long *)pd) [pd_entry] & _PAGE_PRESENT)) {
    if (!(pt = (unsigned long) (*alloc_page) ())) return 0;
    fill_pd_entry (pd, pd_entry,
		   pt, _PAGE_PRESENT | _PAGE_RW | _PAGE_USER);
  } else
    pt = get_pd_addr(pd, pd_entry);

  // Check whether there is already an allocated page in the pt table
  if (!(((unsigned long *)pt)[pt_entry] & _PAGE_PRESENT)) {
    if (!(page = (unsigned long) (*alloc_page) ())) return 0;
    fill_pt_entry (pt, pt_entry, page,
		   _PAGE_PRESENT | _PAGE_RW | _PAGE_USER);
  }
  return page;
}

static inline int free_user_page (unsigned long pd,
				  unsigned long vaddress,
				  void (*free_page) (unsigned long)) {
  unsigned long pt_entry = va2pt(vaddress),
    pd_entry = va2pd(vaddress), pt, page;
  int n;

  if (vaddress >= PAGE_OFFSET) return -1;

  if ((((unsigned long *)pd) [pd_entry] & _PAGE_PRESENT) == _PAGE_PRESENT) {
    pt = get_pd_addr (pd, pd_entry);
    if ((((unsigned long *)pt) [pt_entry] & _PAGE_PRESENT) == _PAGE_PRESENT) {
      page = get_pt_addr (pt, pt_entry);
      ((unsigned long *)pt)[pt_entry] = 0;
      if((((unsigned long *)pt)[pt_entry] & _PAGE_UNUSED1) != _PAGE_UNUSED1) {
	      (*free_page) (page);
      } 
    }

    for (n = 0; n < 1024; n++) {
      if (((unsigned long *)pt)[n]) break;
    }
    if (n == 1024) {
      ((unsigned long *)pd) [pd_entry] = 0;
      (*free_page) (pt);
    }
  }
  return 0;
}

static inline int alloc_user_range (unsigned long pd, unsigned long address,
				    unsigned long offset, 
				    unsigned long (*a_pg) (void)) {
  unsigned long addr, start, end;
  
  start = (address & PAGE_MASK);
  end = address + offset;
  end = !(end & ~PAGE_MASK)? end: (end & PAGE_MASK) + PAGE_SIZE;

  for (addr = start; addr < end; addr += PAGE_SIZE)
    if (allocate_user_page (pd, addr, a_pg) <= 0) return -1;
  
  return 0;
}

static inline int free_user_range (unsigned long pd, unsigned long start,
				   unsigned long offset, 
				   void (*r_pg) (unsigned long)) {
  unsigned long addr, begining = (start & PAGE_MASK),
    end = ((start + offset) % PAGE_SIZE)?
    ((start + offset) & PAGE_MASK) + 0x1000: (start + offset);
  
  for (addr = begining; addr < end; addr += PAGE_SIZE)
    if (free_user_page (pd, addr, r_pg) != 0) return -1;
  return 0;
}


static inline int destroy_page_directory (unsigned long pd,
					  void (*f_pg)(unsigned long)) {
  unsigned long vaddr;

  if (!pd) return -1;
  for (vaddr = 0; vaddr < PAGE_OFFSET; vaddr += PAGE_SIZE)
    free_user_page (pd, vaddr, f_pg);

  (*f_pg) (pd);
  return 0;
}

static inline unsigned long alloc_free_user_range (unsigned long pd,
						   unsigned long vaddr,
						   unsigned long vsize,
						   unsigned long (*a_pg)
						   (void)){
  unsigned long addr, start, end, nvaddr;

  if (!pd) return -1;
  
  start = (vaddr & PAGE_MASK);
  end = vaddr + vsize;
  end = !(end & ~PAGE_MASK)? end: (end & PAGE_MASK) + PAGE_SIZE;

  // Firstly, we have to find a free range of memory, after we will
  // allocate the memory
  nvaddr = start;
  for (addr = start; addr < end && addr < PAGE_OFFSET; addr += PAGE_SIZE) {
    unsigned long pt, pt_entry = va2pt(addr),
      pd_entry = va2pd(addr);
    if ((((unsigned long *)pd) [pd_entry] & _PAGE_PRESENT) == _PAGE_PRESENT) {
      pt = get_pd_addr (pd, pd_entry);
      if ((((unsigned long *)pt) [pt_entry] & 
	   _PAGE_PRESENT) == _PAGE_PRESENT) {
	addr += PAGE_SIZE;
	nvaddr = start = (addr & PAGE_MASK);
	end = addr + vsize;
	end = !(end & ~PAGE_MASK)? end: (end & PAGE_MASK) + PAGE_SIZE;
	
      }
    }
  }
  if (addr >= PAGE_OFFSET) return 0;
  if (alloc_user_range (pd, nvaddr, vsize, a_pg)) return 0;
  
  return nvaddr;
}

#endif

