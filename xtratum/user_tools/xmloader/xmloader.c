/*
 * $FILE: xmloader.c
 *
 * An elf loader
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <string.h>
#include <libgen.h>
#include <sys/mman.h>

#include <xm_syscalls.h>

#define UNLOAD_PROGRAM "unloader.xm"
#define LOAD_PROGRAM "loader.xm"

int process_img (char *filename, domain_image_t *d_img) {
  int status = 0, fd, lock = 0, n;
  Elf32_Ehdr e_hdr;
  Elf32_Phdr *e_phdr;

  if ((fd = open (filename, O_RDONLY)) < 0) {
    status = -1;
    goto out;
  }
 
  if (flock(fd, LOCK_EX)) goto out;
  lock = 1;
  
  read (fd, (char *) &e_hdr, sizeof (Elf32_Ehdr));

  if (memcmp (e_hdr.e_ident, ELFMAG, SELFMAG)) {
    status = -2;
    goto out;
  }
  
  if (e_hdr.e_ident [EI_CLASS] != ELFCLASS32 ||
      e_hdr.e_ident [EI_DATA] != ELFDATA2LSB ||
      e_hdr.e_ident [EI_VERSION] != EV_CURRENT) {
    status = -3;
    goto out;
  }

  if (e_hdr.e_type != ET_EXEC) {
    status = -4;
    goto out;
  }

  if (e_hdr.e_machine != EM_386) {
    status = -5;
    goto out;
  } 
  
  if (e_hdr.e_phoff) {
    e_phdr = (Elf32_Phdr *) malloc (e_hdr.e_phentsize*e_hdr.e_phnum);
    lseek(fd, e_hdr.e_phoff, SEEK_SET);
    read (fd, (char *) e_phdr, sizeof (Elf32_Phdr)*e_hdr.e_phnum);

  } else
    e_phdr = 0;
  
  // Loading the image
  d_img -> section = 0;
  d_img -> nsec = 0;
  d_img -> entry_point = e_hdr.e_entry;

  for (n = 0; n  < e_hdr.e_phnum; n++)
    switch (e_phdr [n].p_type) {
    case PT_NULL: // Ignored
      break;
    case PT_INTERP: // An interpreter?? 
    case PT_DYNAMIC: // Dynamic information???, xm doesn't understand it
      status = -6;
      goto out;
      break;
    case PT_LOAD: // This section must be loaded
      d_img -> nsec ++;
      d_img -> section = realloc 
	(d_img -> section, 
	 d_img -> nsec * sizeof (elf_section_t));

      d_img -> section [d_img -> nsec - 1].img = 
	(unsigned char *) malloc (e_phdr [n].p_memsz);

      d_img -> section [d_img -> nsec - 1].st_addr = 
	e_phdr [n].p_vaddr;

      d_img -> section [d_img -> nsec - 1].size = 
	e_phdr [n].p_memsz;

      d_img -> section [d_img -> nsec - 1].flags = 
	e_phdr [n].p_flags;
      
      d_img -> section [d_img -> nsec - 1].align = 
	e_phdr [n].p_align;
      
      lseek(fd, e_phdr[n].p_offset, SEEK_SET);
      read (fd, (unsigned char *) d_img -> section 
	    [d_img -> nsec - 1].img, e_phdr [n].p_filesz);
      
      break;
    }
  
 out:
  
  if (fd >= 0) {
    if (lock) flock(fd, LOCK_UN);
    close (fd);
  }

  return status;
}

static inline void show_error (int error) {
  char error_string[7][64] = {
    {"No error detected"}, // 0
    {"File not found"}, // 1
    {"This is not an elf file"}, // 2
    {"Elf version wrong"}, // 3
    {"This elf file is not an executable"}, // 4
    {"This elf file was not compiled for the i386 architecture"}, // 5
    {"This elf file contains dynamic sections"}  // 6
  };

  fprintf (stderr, "\n>>> Error: %s\n\n", error_string[-error]);
}

static inline unsigned long domain_img_size (domain_image_t *d_img) {
  unsigned long size, sec;
  
  size = sizeof (domain_image_t) + 
    (sizeof (elf_section_t) * d_img -> nsec);

  for (sec = 0; sec < d_img -> nsec; sec++) 
    size +=  d_img -> section [sec].size;
  
  return size;
}

int main (int argc, char **argv) {
  char *filename = NULL, *dname = NULL;
  domain_image_t d_img;
  int error, args, prio, id = 0;
  if (mlockall (MCL_CURRENT | MCL_FUTURE) != 0) return -1;
  if (argc < 2) {
    if (!strcmp(basename (argv[0]), UNLOAD_PROGRAM))
      fprintf (stderr, "\n\tUse: %s -id <domain_id>\n\n", basename(argv[0]));
    else
      fprintf (stderr, "\n\tUse: %s <domain_image> [-prio <prio>] [-dname <domain_name>]\n\n", basename(argv[0]));
    return -1;
  }

  prio = 30;
  
  for (args = 1; args < argc; args++) {
    if (strcmp(argv[args], "-prio") == 0) {
      if (++args < argc)  prio = atoi (argv[args]);
      else {
	fprintf (stderr, "Error: No priority provided\n");
	return -1;
      }
      continue;
    }
    if (strcmp(argv[args], "-dname") == 0) {
      if (++args < argc)  dname = argv[args];
      else {
	fprintf (stderr, "Error: No domain name provided\n");
	return -1;
      }
      continue;
    }
    if (strcmp(argv[args], "-id") == 0) {
      if (++args < argc)  id = atoi(argv[args]);
      else {
	fprintf (stderr, "Error: No domain id provided\n");
	return -1;
      }
      continue;
    }
    filename = argv[args];
  }

  if (!dname) dname = basename(filename);

  if (strcmp(basename (argv[0]), UNLOAD_PROGRAM)) {
    if (!filename) {
      fprintf (stderr, "Error: No filename provided\n");
      return -1;
    }
  }
  
  if (!strcmp(basename (argv[0]), UNLOAD_PROGRAM)) {
    if (!id) {
      fprintf (stderr, 
	       "Error: Trying to unloaded domain 0 or not any id provided\n");
      return -1;
    }

    fprintf (stderr, ">> Unloading the domain \"(%d)\" ... ", id);
    error = unload_domain (id);
    if (!error)
      fprintf (stderr, "OK\n");
    else {
      fprintf (stderr, "FAILED\n");
      fprintf (stderr, "(%d) does not exit\n", id);
    }
  } else {
    if ((error = process_img (filename, &d_img)) < 0) {
      show_error (error);
      return -1;
    }
    d_img.img_size = domain_img_size (&d_img);
    fprintf (stderr, ">> Loading the domain \"%s (%s)\" ... ", 
	     dname, basename(filename));
    error = load_domain (dname, prio, &d_img);
    if (error >= 0)
      fprintf (stderr, "Ok (Id: %d)\n", error);
    else
      fprintf (stderr, "Failed (Error: %d)\n", error);
  }
  
  return 0;
}
