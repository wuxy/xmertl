/*
 * $FILE: multiboot.h
 *
 * Multiboot header file
 *
 * The multiboot specification can be found at:
 * http://www.gnu.org/software/grub/manual/multiboot/
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

#ifndef _MULTIBOOT_H_
#define _MULTIBOOT_H_

#ifndef _KERNEL_
#error Internal file. Do not include it in your sources.
#endif

/* The magic number for the Multiboot header. */
#define __MULTIBOOT_HEADER_MAGIC 0x1BADB002

/* The flags for the Multiboot header. */
# define __MULTIBOOT_HEADER_FLAGS 0x3

/* The magic number passed by a Multiboot-compliant boot loader. */
#define __MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

#ifndef __ASSEMBLY__

/* The section header table for ELF. */
typedef struct elf_section_header_table {
  unsigned long num;
  unsigned long size;
  unsigned long addr;
  unsigned long shndx;
} elf_section_header_table_t;

/* The Multiboot information. */
typedef struct multiboot_info {
  unsigned long flags;
  unsigned long mem_lower;
  unsigned long mem_upper;
  unsigned long boot_device;
  unsigned long cmdline;
  unsigned long mods_count;
  unsigned long mods_addr;
  elf_section_header_table_t elf_sec;
  unsigned long mmap_length;
  unsigned long mmap_addr;
} multiboot_info_t;

/* The module structure. */
typedef struct module {
  unsigned long mod_start;
  unsigned long mod_end;
  unsigned long string;
  unsigned long reserved;
} multiboot_module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size. */
typedef struct memory_map {
  unsigned long size;
  unsigned long base_addr_low;
  unsigned long base_addr_high;
  unsigned long length_low;
  unsigned long length_high;
  unsigned long type;
} multiboot_memory_map_t;

#endif // !__ASSEMBLY__
#endif // _MULTIBOOT_H_
