/*
 * $FILE: sysmem.c
 *
 * Managing the system memory
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

//#include <vmm.h>
#include <config.h>
#include <arch/linux_syscalls.h>
#include <linkage.h>
#include <tlsf.h>
#include <arch/sysmem.h>

// Definitions

#define SYSTEM_MEMORY_SIZE 8*1024*1024

#define MIN_SIZE 12
static unsigned int HEADER_SIZE;
static list_header_t list;

//----------//
// init_vmm //
//----------//

/* Gathering information about free memory in Linux is fairly simple,
   we just ask for this memory to the Linux kernel via the mmap2
   syscall */

unsigned long init_vmm(void) {
  unsigned char *mem;
  list_t *r;

  { // required by the list used to store the free memory
    list_t l;
    HEADER_SIZE = ((unsigned int)&l.mem.ptr[0] - 
      (unsigned int) &l);
    list.head = NULL;
  }

  mem = linux_mmap2 (0, SYSTEM_MEMORY_SIZE, 
		     PROT_EXEC | PROT_READ | PROT_WRITE, 
		     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  
  if (mem == ((void *) -1))
    return 0;
  

  r = (list_t *)mem;
  r -> size = SYSTEM_MEMORY_SIZE - HEADER_SIZE;
  SET_FREE_BLOCK (r);
  
  r -> mem.free_ptr.next = r -> mem.free_ptr.prev = 
    r -> prev_phys = r -> next_phys = NULL;

  list.head = r;
  
  return r -> size;
}

//--------------//
// alloc_region //
//--------------//

void *alloc_region (unsigned long from, unsigned long to, unsigned long size) {
  list_t *aux, *pos = NULL, *new;
  unsigned int new_size;
  
  if (!size) return (void *) NULL;
  
  if (size < MIN_SIZE) size = MIN_SIZE;
  
  aux = list.head;

  for ( ; aux ;  aux = aux -> mem.free_ptr.next){
      if (aux->size >= size) {
	  pos = aux;
	  break;
      }
  }
  
  aux = pos;

  if (!aux) return (void *) NULL;
  
  if (aux -> mem.free_ptr.next)
    aux -> mem.free_ptr.next -> mem.free_ptr.prev = aux -> mem.free_ptr.prev;

  if (aux -> mem.free_ptr.prev)
    aux -> mem.free_ptr.prev -> mem.free_ptr.next = aux -> mem.free_ptr.next;
  
  if (list.head == aux)
    list.head = aux -> mem.free_ptr.next;
  
  SET_USED_BLOCK (aux);
  aux -> mem.free_ptr.next = NULL;
  aux -> mem.free_ptr.prev = NULL;
  
  new_size = GET_BLOCK_SIZE(aux) - size - HEADER_SIZE;
  if (((int) new_size) >= (int)MIN_SIZE) {
    new = (list_t *) (((char *) aux) + (unsigned long) HEADER_SIZE + 
		      (unsigned long) size);
    new -> size = new_size;
    new -> mem.free_ptr.prev = NULL;
    new -> mem.free_ptr.next = NULL;

    SET_FREE_BLOCK (new);

    new -> prev_phys = aux;
    aux -> next_phys = new;

    new -> next_phys = aux -> next_phys;
    if (aux -> next_phys)
      aux -> next_phys -> prev_phys = new;

    aux -> size = size;
    SET_USED_BLOCK (aux);
    // the new block is indexed inside of the list of free blocks
    new -> mem.free_ptr.next = list.head;
    if (list.head)
      list.head -> mem.free_ptr.prev = new;
    list.head = new;

  }

  return (void *) &aux -> mem.ptr [0];
}

//----------------//
// dealloc_region //
//----------------//

void dealloc_region (void *ptr, unsigned long size) {
  list_t *b = (list_t *) ((char *)ptr - HEADER_SIZE), *b2;

  if (!ptr || !IS_USED_BLOCK(b))
    return;
  
  SET_FREE_BLOCK (b);
  b -> mem.free_ptr.next = NULL;
  b -> mem.free_ptr.prev = NULL;
  if (b -> prev_phys) {
    b2 = b -> prev_phys;
    if (!IS_USED_BLOCK (b2)) {
      b2 -> size = GET_BLOCK_SIZE(b2) + GET_BLOCK_SIZE (b) + HEADER_SIZE;
      if (b2 -> mem.free_ptr.next)
	b2 -> mem.free_ptr.next -> mem.free_ptr.prev = b2 -> mem.free_ptr.prev;

      if (b2 -> mem.free_ptr.prev)
	b2 -> mem.free_ptr.prev -> mem.free_ptr.next = b2 -> mem.free_ptr.next;
     
      if (list.head == b2)
	list.head = b2 -> mem.free_ptr.next;
      SET_FREE_BLOCK (b2);
      b2 -> mem.free_ptr.next = NULL;
      b2 -> mem.free_ptr.prev = NULL;

      if (b -> next_phys)
	b -> next_phys -> prev_phys = b2;
      b2 -> next_phys = b -> next_phys;

      b = b2;
    }
  }

  if (b -> next_phys) {
    b2 = b -> next_phys;
    
    if (!IS_USED_BLOCK (b2)) {
      b -> size += GET_BLOCK_SIZE(b2) + HEADER_SIZE;
      
      if (b2 -> mem.free_ptr.next)
	b2 -> mem.free_ptr.next -> mem.free_ptr.prev = b2 -> mem.free_ptr.prev;
      
      if (b2 -> mem.free_ptr.prev)
	b2 -> mem.free_ptr.prev -> mem.free_ptr.next = b2 -> mem.free_ptr.next;
      
      if (list.head == b2)
	list.head = b2 -> mem.free_ptr.next;

      b2 -> mem.free_ptr.next = NULL;
      b2 -> mem.free_ptr.prev = NULL;

      if (b2 -> next_phys)
	b2 -> next_phys -> prev_phys = b;
      b -> next_phys = b2 -> next_phys;
    }

    b -> mem.free_ptr.next = list.head;
  
    if (list.head)
      list.head -> mem.free_ptr.prev = b;
    list.head = b;
  }
}

//----------------//
// create_app_map //
//----------------//

void create_app_map (void) {
}

//------------//
// ualloc_sys //
//------------//

asmlinkage void *ualloc_sys (int size) {
  char *mem;
  mem = linux_mmap2 (0, size,
		     PROT_EXEC | PROT_READ | PROT_WRITE, 
		     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  
  if (mem == ((void *) -1))
    return 0;
  return mem;
}

//-----------//
// ufree_sys //
//-----------//

asmlinkage void ufree_sys (void *ptr, int size) {
  linux_munmap (ptr, size);
}

//-------------//
// init_sysmem //
//-------------//

unsigned long init_sysmem (void) {
  unsigned long freemem = init_vmm ();
  
  if (CONFIG_KDYNAMIC_MEMORY > freemem)
  	return -1;

  if (!(memory_pool = alloc_region (0, 0xffffffff, CONFIG_KDYNAMIC_MEMORY)))
	return -1;

  // Setting up TLSF with the largest free area, memory_pool will be
  // zeroed by this function as well
  printf ("\nSetting up the dynamic memory manager (%d kbytes at 0x%x)\n", 
	  CONFIG_KDYNAMIC_MEMORY/1024, memory_pool);
  if (init_memory_pool (CONFIG_KDYNAMIC_MEMORY, memory_pool) == 0)
  	return -1;

  return freemem;
}
