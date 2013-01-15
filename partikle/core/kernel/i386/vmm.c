/*
 * $FILE: vmm.c
 *
 * Virtual memory manager
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

#include <config.h>
#include <sysmemory.h>
#include <stddef.h>
#include <cbitmap.h>
#include <processor.h>
#include <tlsf.h>
#include <arch/multiboot.h>
#include <arch/paging.h>

// Here it is our memory map
extern multiboot_info_t *mbi;
extern unsigned long _sfreemem;

//------//
// skip //
//------//

#define skip(hole_min, hole_max) \
  if ((max > (hole_min)) && (min < (hole_max))) { \
    if (min < (hole_min)) max = (hole_min); \
    else { min = (hole_max); goto retry; } \
  }


#ifdef CONFIG_X86_PAGING

// MEMORY MAP when paging is enabled

/*
 *   -------------------- 0xFFFFFFFF
 *   |    FREE MEMORY   |
 *   -------------------- 0xC0800000
 *   |      PGTs        |
 *   -------------------- 0xC0400000
 *           ...
 *   |     Page table   |
 *   --------------------
 *   |     Kernel       |
 *   -------------------- 0xC0100000
 *   |       BIOS       |
 *   -------------------- 0xC0000000 (Kernel)
 *   |                  |
 *           ...         
 *   |                  |
 *   -------------------- 0x00000000 (App.)
 */

static cbitmap_t page_table;

#define FREE_VMM_ENTRY (KERNEL_PGD_START + 2)

#define BIT2ADDR(bitp) (bitp * PAGE_SIZE)
#define ADDR2BIT(addr) (addr/PAGE_SIZE)

//------------//
// add_region //
//------------//

static unsigned long add_region (unsigned long addr, unsigned long size) {
  unsigned long vaddr = ROUNDUP2PAGE(addr);
  int npages = (size - (vaddr - addr)) / PAGE_SIZE, n;
  
  for (n = 0; n < npages; n++, vaddr += PAGE_SIZE)
    setbit_cbitmap (&page_table, ADDR2BIT(vaddr));

  return npages * PAGE_SIZE;
}

//----------//
// init_vmm //
//----------//

unsigned long init_vmm (void) {
  unsigned long free_memory = 0, min;
  unsigned long sysmem = 0, pgt_e, addr;

  // the lowest part of the memory map is not longer mapped
  pgd [0] = 0;

  // we also unmap all the memory that is not going to be used
  for (pgt_e = va2pgt((unsigned long) &_sfreemem + OFFSET);
       pgt_e < PAGE_DT_ENTRIES; pgt_e ++)
    pg0 [pgt_e] = 0;

  // we map all pgts starting at 0xC0400000 
  // mapping pg_pgt at 0xC0400000
  pgd [PGTS_ENTRY] = 
    (VIRT2PHYS(pg_pgt) | _PG_PRESENT | _PG_GLOBAL | _PG_RW);

  // mapping pgts at that address
  pg_pgt [KERNEL_PGD_START] = 
    (VIRT2PHYS(pg0) | _PG_PRESENT | _PG_GLOBAL | _PG_RW);
  pg_pgt [KERNEL_PGD_START + 1] =
    (VIRT2PHYS(pg_pgt) | _PG_PRESENT | _PG_GLOBAL | _PG_RW);

  // mbi points out a non mapped area
  // updating mbit with the correct address
  mbi = (multiboot_info_t *)((unsigned long) mbi + OFFSET);
  
  if (mbi -> flags & 0x1) {
    // mapping memory for the page table
    sysmem = 0x100000 + mbi -> mem_upper * 1024;

    // the page table never should exceed the OFFSET + 4 Mbytes
    // Indeed 4 mbytes should be enough to store the kernel and the
    // paget table structure
    if (va2pgd((PHYS2VIRT((unsigned long) &_sfreemem) + 
	       sysmem/PAGE_SIZE)) > PAGE_DT_ENTRIES) return -1;
    
    // mapping memory for the page table structure
    for (pgt_e = va2pgt(PHYS2VIRT((unsigned long) &_sfreemem)),
	   addr = (unsigned long) &_sfreemem;
	 pgt_e <= va2pgt(PHYS2VIRT((unsigned long) &_sfreemem)
			+ ARRAY_SIZE (sysmem/PAGE_SIZE) 
			* sizeof (unsigned long));
	 pgt_e ++, addr += PAGE_SIZE)
      pg0 [pgt_e] =  addr | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
    
    init_cbitmap (&page_table, 
		  (unsigned long *) PHYS2VIRT((unsigned long)&_sfreemem), 
		  sysmem/PAGE_SIZE);
    
    // Checking a hole of memory big enough to be used 
    min = (unsigned long) &_sfreemem +
      + ARRAY_SIZE (sysmem/PAGE_SIZE) * sizeof (unsigned long);
    
    do {
	unsigned long max = 0xffffffff;
	
	skip (0x100000 + mbi -> mem_upper * 1024, 0xffffffff);
	free_memory += add_region (min, max - min);
	min = max;
      retry:
	;
    } while (min < 0xffffffff);
  }

  flush_tlb_all ();
  
  return free_memory;
}

//-----------------//
// alloc_phys_page //
//-----------------//

// this page could be not mapped
unsigned long alloc_phys_page (void) {
  int bit, iflags;

  hw_save_flags_and_cli (iflags);
  bit = findfbs_cbitmap (&page_table);
  clearbit_cbitmap (&page_table, bit);
  hw_restore_flags (iflags);

  if (bit == -1)
    panic (0, "Out of physical memory\n");
  

  return BIT2ADDR(bit);
}

//-------------------//
// dealloc_phys_page //
//-------------------//

void dealloc_phys_page (unsigned long addr) {
  int iflags;
 
  hw_save_flags_and_cli (iflags);
  setbit_cbitmap (&page_table, ADDR2BIT(addr));
  hw_restore_flags (iflags);
}

//--------------//
// alloc_region //
//--------------//

void *alloc_region (unsigned long from, unsigned long to, 
		    unsigned long size) {
  unsigned long addr, saddr;
  int pgd_e, pgt_e;
  unsigned long page;
  unsigned int *pgt;

  if (!from)
    from = pgdpgt2va (FREE_VMM_ENTRY, 0);
  
  if (!to)
    to = 0xffffffff;
  
  if ((from & (PAGE_SIZE - 1))) 
    return 0;

  for (addr = saddr = from; 
       addr < to; addr += PAGE_SIZE) {

    pgd_e = va2pgd (addr);
    pgt_e = va2pgt (addr);
    
    if (pgd [pgd_e]) {
      pgt = (unsigned int *) pgdpgt2va (PGTS_ENTRY, pgd_e);
      // this address is already in use
      if (pgt [pgt_e] & PAGE_MASK) {
	saddr = addr + PAGE_SIZE;
	goto next;
      }
    }
    if ((addr - saddr) >= size) goto found;
  next:
    ;
  }
  // no empty space in the virtual memory map
  return NULL;

 found:
  
  for (addr = saddr; addr < saddr + size; addr += PAGE_SIZE) {
    pgd_e = va2pgd (addr);
    pgt_e = va2pgt (addr);
    if (!pgd[pgd_e]) {
      page = alloc_phys_page ();
      pgd [pgd_e] = page | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
      pg_pgt [pgd_e] = page | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
      pgt = (unsigned int *)pgdpgt2va (PGTS_ENTRY, pgd_e);
      CLEAN_PAGE (pgt);
    }
    pgt = (unsigned int *)pgdpgt2va (PGTS_ENTRY, pgd_e);
    
    page = alloc_phys_page ();
    if (pgt [pgt_e] & PAGE_MASK) {
      panic (0, "Mapping memory in a already mapped area\n");
    }
    pgt [pgt_e] = page | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
  }

  flush_tlb_all ();

  return (void *)saddr;
}

//----------------//
// dealloc_region //
//----------------//

/*
 * TODO: This is a very simple version where only the pgt page is
 * unmapped but not the pgt itself
 */

void dealloc_region (void *saddr, unsigned long size) {
  unsigned long addr; 
  int pgd_e, pgt_e;
  unsigned int *pgt;
  
  if ((unsigned long)saddr & (PAGE_SIZE - 1)) 
    return;
  
  for (addr = (unsigned long) saddr; 
       addr < (unsigned long) saddr; addr += PAGE_SIZE) {
    pgd_e = va2pgd (addr);
    pgt_e = va2pgt (addr);
    pgt = (unsigned int *)pgdpgt2va (PGTS_ENTRY, pgd_e);
    // unmapping the page
    pgt [pgt_e] = 0;
  }

  flush_tlb_all ();
}

//------------//
// ualloc_sys //
//------------//

extern unsigned long _eapp;
asmlinkage void *ualloc_sys (int size) {
  return alloc_region (ROUNDUP2PAGE((unsigned long)&_eapp), 
		       STACK_ADDR-1, size);
}

//-----------//
// ufree_sys //
//-----------//

asmlinkage void ufree_sys (void *ptr, int size) {
  dealloc_region (ptr, size);
}

//----------------//
// create_app_map //
//----------------//

extern unsigned long _sapp, _eapp;

void create_app_map (void) {
  unsigned long begin_app =(unsigned long) &_sapp;
  unsigned long end_app = (unsigned long) &_eapp;
  unsigned long page, addr;
  int pgd_e, pgt_e;
  unsigned int *pgt;

  end_app = ROUNDUP2PAGE (end_app);
  
  for (addr = begin_app; addr < end_app; addr += PAGE_SIZE) {
    pgd_e = va2pgd (addr);
    pgt_e = va2pgt (addr);

    if (!pgd[pgd_e]) {
      page = alloc_phys_page ();
      pgd [pgd_e] = page | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
      pg_pgt [pgd_e] = page | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
      pgt = (unsigned int *)pgdpgt2va (PGTS_ENTRY, pgd_e);
      CLEAN_PAGE (pgt);
    }
    pgt = (unsigned int *)pgdpgt2va (PGTS_ENTRY, pgd_e);
    pgt [pgt_e] = addr | _PG_PRESENT | _PG_GLOBAL | _PG_RW;
  }
  flush_tlb_all ();
}

#else

//----------//
// init_vmm //
//----------//

static unsigned long init_vmm (void) {
  unsigned long free_memory = 0, min;
  unsigned long sysmem = 0;


  mbi = (multiboot_info_t *)((unsigned long) mbi + OFFSET);
  
  if (mbi -> flags & 0x1) {
    // mapping memory for the page table
    sysmem = 0x100000 + mbi -> mem_upper * 1024;
    
    // Checking a hole of memory big enough to be used 
    min = (unsigned long) &_sfreemem;
    
    do {
	unsigned long max = 0xffffffff;
	
	skip (0x100000 + mbi -> mem_upper * 1024, 0xffffffff);
	free_memory += (max - min);
	min = max;
      retry:
	;
    } while (min < 0xffffffff);
  }

  return free_memory;
}

//--------------//
// alloc_region //
//--------------//

static void *alloc_region (unsigned long from, unsigned long to, 
			   unsigned long size) {
  return (void *) &_sfreemem;
}

//----------------//
// dealloc_region //
//----------------//

//static void dealloc_region (void *saddr, unsigned long size) {
//}

//------------//
// ualloc_sys //
//------------//

asmlinkage void *ualloc_sys (int size) {
  return malloc_ex (size, memory_pool);
}

//-----------//
// ufree_sys //
//-----------//

asmlinkage void ufree_sys (void *ptr, int size) {
  free_ex (ptr, memory_pool);
}

//----------------//
// create_app_map //
//----------------//

extern unsigned long _sapp, _eapp;

void create_app_map (void) {
}

#endif

//-------------//
// init_sysmem //
//-------------//

unsigned long init_sysmem (void) {
  unsigned long freemem = init_vmm ();
  
  if (CONFIG_KDYNAMIC_MEMORY > freemem) return -1;

  if (!(memory_pool = alloc_region
	(0, 0xffffffff, CONFIG_KDYNAMIC_MEMORY))) return -1;

  // Setting up TLSF with the largest free area, memory_pool will be
  // zeroed by this function as well
  printf ("\nSetting up the dynamic memory manager (%d kbytes at 0x%x)\n", 
	  CONFIG_KDYNAMIC_MEMORY/1024, memory_pool);
  if (init_memory_pool (CONFIG_KDYNAMIC_MEMORY, memory_pool) == 0) return -1;

  return freemem;
}
