/*
 * $FILE: start.S
 *
 * Entry point of the kernel in the i386 architecture version
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
#include <arch/irqs.h>
#include <arch/multiboot.h>
#include <arch/paging.h>
#include <arch/processor.h>
	
.section .multiboot_hdr, "ax"

.globl start, _start
.align 4

start:
_start:
	jmp _multiboot_entry

.align 4
_multiboot_header:
/* magic */
.long __MULTIBOOT_HEADER_MAGIC
/* flags */
.long __MULTIBOOT_HEADER_FLAGS
/* checksum */
.long -(__MULTIBOOT_HEADER_MAGIC + __MULTIBOOT_HEADER_FLAGS)

_multiboot_entry:
	cld
	/* compare the multiboot magic number with eax */
	cmp $__MULTIBOOT_BOOTLOADER_MAGIC,%eax
	je 1f

	/* 
	 * if  the  returned value  isn't  valid  then the machine  is
	 * halted with a  beep.  Right now, here  it is the only place of
	 * the whole code where partikle makes beep. 
	 */
	jmp _VIRT2PHYS(__halt_beeping)

	/* turning off all floppy units */
1:	xorb    %al,%al
	movw    $0x3f2,%dx /* all floppies on primary adapter */
	outb    %al,%dx
	movw    $0x372,%dx /* all floppies on secondary adapter */
	outb    %al,%dx

	/* 
	 * the multiboot standard mentions that the bss is already zeroed,
	 * it might better to clear it again, however. 
	 */
	xorl %eax, %eax
	movl $_VIRT2PHYS(_sbss), %edi
	movl $_VIRT2PHYS(_ebss), %ecx
	subl %edi, %ecx
	rep
	stosb

	/* 
	 * multiboot information could be pretty useful,
	 * we store it inside the mbi variable
	 */
	mov %ebx, _VIRT2PHYS(mbi)

	/* setting up a valid gdt */
	lgdt phys_gdt_desc

	/* loading descriptors from our new gdt */
	ljmp $(KERNEL_CS),$1f
	
1:	movl $(KERNEL_DS), %eax
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %ss

#ifdef CONFIG_X86_PAGING
	/* filling the virtual memory pages */
	movl $_VIRT2PHYS(pg0),%edi

	/* mapped pages have next flags: SUPERVISOR + RW + PRESENT + GLOBAL */
	movl $(_PG_PRESENT | _PG_RW | _PG_GLOBAL),%eax

2:	stosl
	add $0x1000,%eax
	cmp $_VIRT2PHYS(pg0) + PAGE_SIZE,%edi
	jne 2b

	mov $_VIRT2PHYS(pg0), %ebx

	/* this page has next attributes: SUPERVISOR + RW + PRESENT */
	add $(_PG_PRESENT | _PG_RW), %ebx
	xorl %eax,%eax
	mov %ebx, _VIRT2PHYS(pgd)(,%eax,4)
	mov $KERNEL_PGD_START,%eax
	mov %ebx, _VIRT2PHYS(pgd)(,%eax,4)

	/* enabling paging */
	movl $_VIRT2PHYS(pgd), %eax
	movl %eax, %cr3
	movl %cr0,%eax
	orl $0x80000000,%eax
	movl %eax,%cr0
#endif
	
	/* once paging is enabled, it is possible to jump
	 * to the actual partikle address (0xC0000000)
	 */
	jmp enter_partikle

	.word 0
phys_gdt_desc:
	.word GDT_ENTRIES*8-1
	.long (_VIRT2PHYS(gdt))
