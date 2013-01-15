/*
 * $FILE: kmem.c 
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

#include <arch/kmem.h>

unsigned long kmem_kvaddr_to_kladdr(unsigned long kvaddr)
{
	unsigned long paddr = 0UL;
	pgd_t *pgd; 
	pud_t *pud;
	pmd_t *pmd;
	pte_t pte;

	pgd = pgd_offset_k(kvaddr);

	if(! pgd_none(*pgd)) {
		pud = pud_offset(pgd, kvaddr);
		
		if(!pud_none(*pud)) {
			pmd = pmd_offset(pud, kvaddr);
			
			if(!pmd_none(*pmd)) {
			
				pte = *(pte_offset_kernel(pmd, kvaddr));
				
				if(pte_present(pte)) {
					
					paddr = (unsigned long) page_address(pte_page(pte));
					paddr |= (kvaddr & (PAGE_SIZE - 1));
				}
			}
		}
	}

	return paddr;    
}

EXPORT_SYMBOL(kmem_kvaddr_to_kladdr);


/*
 * Routine: kmem_page_map
 *
 * parameters:
 *   pdCRC:
 *   flags:
 *   dvaddr:
 *   paddr:
 *
 * return value:
 *   failure:
 *   successful:
 *   
 * */
unsigned long kmem_page_map(unsigned long pdCRC, unsigned long flags, 
		unsigned long dvaddr, unsigned long kladdr)
{
	unsigned long pd_entry = va2pd(dvaddr);
	unsigned long pt_entry = va2pt(dvaddr);
	unsigned long pt;


	/* Fill the middle level page table */

	if( !((((unsigned long *)pdCRC)[pd_entry]) & _PAGE_PRESENT) ) {
		pt = __get_free_page(GFP_ATOMIC);

		if( !pt ) return 0UL;

		memset((unsigned char *)pt, 0, PAGE_SIZE);

		fill_pd_entry(pdCRC, pd_entry, pt, (flags | _PAGE_PRESENT));
	} else {
		pt = get_pm_addr(pdCRC, pd_entry);
	}


	if ( !(((unsigned long *)pt)[pt_entry] & _PAGE_PRESENT)) {
		fill_pt_entry (pt, pt_entry, kladdr, flags);
	
		return dvaddr;
	}

	return 0UL;
}

EXPORT_SYMBOL(kmem_page_map);


/*
 * Routine: kmem_page_unmap
 *
 * parameters:
 *   pdCRC:
 *   dvaddr:
 *
 * return value:
 *   failure:
 *   succesful:
 *
 * */
int kmem_page_unmap(unsigned long pdCRC, unsigned long dvaddr)
{
	unsigned long pd_entry = va2pd(dvaddr);
	unsigned long pt_entry = va2pt(dvaddr);
	unsigned long pt;

	if (((unsigned long *)pdCRC)[pd_entry] & _PAGE_PRESENT ) {
		pt = get_pm_addr(pdCRC, pd_entry);
		
		if(((unsigned long *)pt)[pt_entry] & _PAGE_PRESENT) {
			((unsigned long *)pt)[pt_entry] = 0UL;
		}
	}

	return 0;
}

EXPORT_SYMBOL(kmem_page_unmap);

