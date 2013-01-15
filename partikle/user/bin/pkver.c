/*
 * $FILE: xmloader.c
 *
 * A simple ELF section reader (NOTE type sections)
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

int process_img (char *filename) {
  struct magic_note {
    unsigned long namesz;
    unsigned long descsz;
    unsigned long type;
    char vendor[12];
    char arch[8];
    char version[6];
  } magic_note;
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

	for (n = 0; n  < e_hdr.e_phnum; n++)
		switch (e_phdr [n].p_type) {
		case PT_NOTE:
		  if (e_phdr [n].p_filesz!=sizeof(struct magic_note)) 
		    break;
		  lseek(fd, e_phdr[n].p_offset, SEEK_SET);
		  read (fd, (unsigned char *)&magic_note, e_phdr [n].p_filesz);
		  if (strcmp(magic_note.vendor, "PaRTiKle")==0) {
		    fprintf(stderr, "%s: PaRTiKle-%s, %s\n", filename,  magic_note.version, magic_note.arch);
		    status=1;
		  }
		  break;
		}
  
 out:
  
	if (fd >= 0) {
		if (lock) flock(fd, LOCK_UN);
		close (fd);
	}

	return status;
}

static inline void show_error (int error, char *name) {
	char * error_string[7] = {
		"No error detected",						// 0
		"File not found", 						// 1
		"This is not an ELF file", 					// 2
		"ELF version wrong", 						// 3
		"It is not an executable ELF", 					// 4
		"This ELF file was not compiled for the i386 architecture", 	// 5
		"This ELF file contains dynamic sections"  			// 6
	};

	fprintf (stderr, "%s: %s\n", name, error_string[-error]);
}

int main (int argc, char **argv) {
	char *filename = NULL;
	int error, args;
	if (argc < 2) {
		fprintf (stderr, "Extract version information from a PaRTiKle executable.\n"
				 "Usage: %s <file(s)>\n", basename(argv[0]));
		return -1;
	}

  
	for (args = 1; args < argc; args++) {
		filename = argv[args];
		//		fprintf(stderr, "Processing \"%s\":", basename(filename));
		if ((error = process_img (filename))<0) {
		  show_error (error,filename);
		}
		if (!error) {
		  show_error (error,filename);
		}
		
	}
	return 0;
}
