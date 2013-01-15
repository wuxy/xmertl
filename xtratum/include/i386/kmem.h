/*
 * $FILE: kmem.h 
 *
 * $Description:
 *   The kmem is a memory management library which used by XtratuM to 
 *   convert kinds of memory address, such as kernel virtual address, 
 *   kernel logic address, physical address, and domain virtual address.
 *
 * $VERSION: 0.1
 *
 * Author: Bai Shuwei <baishuwi@weslab.com>
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
 */


#ifndef __XM_KMEM_H__

#define __XM_KMEM_H__

#include <linux_inc.h>

#include <arch/mm.h>
/*
 * There are four kinds of address in the library:
 *   1) kernel logic address  : the address is localed in the linux kernel
 *   space. it can be get by __get_free_page(), kmalloc() routines. It will
 *   be named by kladdr in the library.
 *
 *   2) kernel virtual address: the address is localed in the linux kernel 
 *   space too, but it is allocted by routine vmalloc() routine. it will be
 *   named by kvaddr. 
 *
 *   3) domain virtual address: the address is localed in the domain space.
 *   it will be named dvaddr in the library.
 *
 *   4) and physicall address : this is the pysical memory address. It will 
 *   be named paddr.
 *
 * Some specially names are listed here too:
 *   
 *   1) pdCRC   : physical address for the first level page table of domain. 
 *   The value is saved in the CRC register when the domain is switched.
 *
 *   2) pm_entry: the index for the second level page table.
 *
 *   3) pt_entry: the index for the third level page table.
 *
 *   4) pdCRC[pm_entry] which specify the mask flag and the physical address 
 *   for the second level page table). 
 *   
 *   5) pmTab = get_page_table_address(pdCRC[pm_entry]) specify the physical 
 *   address for the second level page table.
 *
 *   6) pmTab[pt_entry] specify the page mask flag and the physical address 
 *   for the third level page table.
 *
 *   7) page = get_page_address(pmTab[pt_entry]); specify the physical address
 *   for the data page.
 *
 * */

#if 0
#define va2pd(vaddr) ((vaddr) >> PGDIR_SHIFT)
#define va2pt(vaddr) (((vaddr) & 0x3FF000) >> PAGE_SHIFT)

#define get_pm_addr(pdCRC, pd_entry) \
	((unsigned long)__va (((unsigned long *)(pdCRC)) [pd_entry] & PAGE_MASK))

#define get_pt_addr(pmTab, pt_entry) \
	((unsigned long)__va (((unsigned long *)(pmTab)) [pt_entry] & PAGE_MASK))

#define kmem_kladdr_to_paddr(kladdr) __pa(kladdr)

static inline void fill_pt_entry (unsigned long pt,
	unsigned long pt_entry, unsigned long page, unsigned long flags) 
{
	((unsigned long *)pt) [pt_entry] = ((__pa (page) & PAGE_MASK) | (flags & 0xFFF));
}

static inline void fill_pd_entry (unsigned long pd,
	unsigned long pd_entry, unsigned long pt, unsigned long flags) 
{
	((unsigned long *)pd) [pd_entry] = ((__pa (pt) & PAGE_MASK) | (flags & 0xFFF));
}
#endif

#define get_pm_addr(pdCRC, pd_entry) \
	((unsigned long)__va (((unsigned long *)(pdCRC)) [pd_entry] & PAGE_MASK))

extern unsigned long kmem_kvaddr_to_kladdr(unsigned long kvaddr);

extern unsigned long kmem_page_map(unsigned long pdCRC, unsigned long flags, 
				unsigned long dvaddr, unsigned long kladdr);
extern int kmem_page_unmap(unsigned long pdCRC, unsigned long dvaddr);


#endif
