/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                               A D A I N T                                *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *          Copyright (C) 1992-2005, Free Software Foundation, Inc.         *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 2,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License *
 * for  more details.  You should have  received  a copy of the GNU General *
 * Public License  distributed with GNAT;  see file COPYING.  If not, write *
 * to  the Free Software Foundation,  59 Temple Place - Suite 330,  Boston, *
 * MA 02111-1307, USA.                                                      *
 *                                                                          *
--
--
--
--
--
--
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

/* This file contains those routines named by Import pragmas in
   packages in the GNAT hierarchy (especially GNAT.OS_Lib) and in
   package Osint.  Many of the subprograms in OS_Lib import standard
   library calls directly. This file contains all other routines.  */

/* We don't have libiberty, so use malloc.  */
#define xmalloc(S) malloc (S)
#define xrealloc(V,S) realloc (V,S)

#include "adaint.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXPATHLEN 100

/* Define symbols O_BINARY and O_TEXT as harmless zeroes if they are not
   defined in the current system. On DOS-like systems these flags control
   whether the file is opened/created in text-translation mode (CR/LF in
   external file mapped to LF in internal file), but in Unix-like systems,
   no text translation is required, so these flags have no effect.  */

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef O_TEXT
#define O_TEXT 0
#endif

#ifndef HOST_EXECUTABLE_SUFFIX
#define HOST_EXECUTABLE_SUFFIX ""
#endif

#ifndef HOST_OBJECT_SUFFIX
#define HOST_OBJECT_SUFFIX ".o"
#endif

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR ':'
#endif

#ifndef DIR_SEPARATOR
#define DIR_SEPARATOR '/'
#endif

char __gnat_dir_separator = DIR_SEPARATOR;

char __gnat_path_separator = PATH_SEPARATOR;

/* The GNAT_LIBRARY_TEMPLATE contains a list of expressions that define
   the base filenames that libraries specified with -lsomelib options
   may have. This is used by GNATMAKE to check whether an executable
   is up-to-date or not. The syntax is

     library_template ::= { pattern ; } pattern NUL
     pattern          ::= [ prefix ] * [ postfix ]

   These should only specify names of static libraries as it makes
   no sense to determine at link time if dynamic-link libraries are
   up to date or not. Any libraries that are not found are supposed
   to be up-to-date:

     * if they are needed but not present, the link
       will fail,

     * otherwise they are libraries in the system paths and so
       they are considered part of the system and not checked
       for that reason.

   ??? This should be part of a GNAT host-specific compiler
       file instead of being included in all user applications
       as well. This is only a temporary work-around for 3.11b.  */

#ifndef GNAT_LIBRARY_TEMPLATE
#if defined (__EMX__)
#define GNAT_LIBRARY_TEMPLATE "*.a"
#elif defined (VMS)
#define GNAT_LIBRARY_TEMPLATE "*.olb"
#else
#define GNAT_LIBRARY_TEMPLATE "lib*.a"
#endif
#endif

const char *__gnat_library_template = GNAT_LIBRARY_TEMPLATE;

/* This variable is used in hostparm.ads to say whether the host is a VMS
   system.  */
const int __gnat_vmsp = 0;

#define GNAT_MAX_PATH_LEN MAXPATHLEN

/* The __gnat_max_path_len variable is used to export the maximum
   length of a path name to Ada code. max_path_len is also provided
   for compatibility with older GNAT versions, please do not use
   it. */

int __gnat_max_path_len = GNAT_MAX_PATH_LEN;
int max_path_len = GNAT_MAX_PATH_LEN;

/* The following macro HAVE_READDIR_R should be defined if the
   system provides the routine readdir_r.  */
#undef HAVE_READDIR_R

#define MAYBE_TO_PTR32(argv) argv

void
__gnat_to_gm_time
  (OS_Time *p_time,
   int *p_year,
   int *p_month,
   int *p_day,
   int *p_hours,
   int *p_mins,
   int *p_secs)
{
  struct tm *res;
  time_t time = (time_t) *p_time;

  res = gmtime (&time);

  if (res)
    {
      *p_year = res->tm_year;
      *p_month = res->tm_mon;
      *p_day = res->tm_mday;
      *p_hours = res->tm_hour;
      *p_mins = res->tm_min;
      *p_secs = res->tm_sec;
    }
  else
    *p_year = *p_month = *p_day = *p_hours = *p_mins = *p_secs = 0;
}

/* Place the contents of the symbolic link named PATH in the buffer BUF,
   which has size BUFSIZ.  If PATH is a symbolic link, then return the number
   of characters of its content in BUF.  Otherwise, return -1.  For Windows,
   OS/2 and vxworks, always return -1.  */

/*int
__gnat_readlink (char *path ,
		 char *buf ,
		 size_t bufsiz )
{}*/

/* Creates a symbolic link named NEWPATH which contains the string OLDPATH.  If
   NEWPATH exists it will NOT be overwritten.  For Windows, OS/2, VxWorks,
   Interix and VMS, always return -1. */

/*
int
__gnat_symlink (char *oldpath ,
		char *newpath )
{
}*/

/* Try to lock a file, return 1 if success.  */

/* Version using link(), more secure over NFS.  */
/* See TN 6913-016 for discussion ??? */

/*int
__gnat_try_lock (char *dir, char *file)
{
}*/

/* Return the maximum file name length.  */

/*int
__gnat_get_maximum_file_name_length (void)
{}*/

/* Return nonzero if file names are case sensitive.  */

/*
int
__gnat_get_file_names_case_sensitive (void)
{
}*/

/*char
__gnat_get_default_identifier_character_set (void)
{
}*/

/* Return the current working directory.  */
/*
void
__gnat_get_current_dir (char *dir, int *length)
{
}
*/
/* Return the suffix for object files.  */

void
__gnat_get_object_suffix_ptr (int *len, const char **value)
{
  *value = HOST_OBJECT_SUFFIX;

  if (*value == 0)
    *len = 0;
  else
    *len = strlen (*value);

  return;
}

/* Return the suffix for executable files.  */

void
__gnat_get_executable_suffix_ptr (int *len, const char **value)
{
  *value = HOST_EXECUTABLE_SUFFIX;
  if (!*value)
    *len = 0;
  else
    *len = strlen (*value);

  return;
}

/* Return the suffix for debuggable files. Usually this is the same as the
   executable extension.  */

void
__gnat_get_debuggable_suffix_ptr (int *len, const char **value)
{
#ifndef MSDOS
  *value = HOST_EXECUTABLE_SUFFIX;
#else
  /* On DOS, the extensionless COFF file is what gdb likes.  */
  *value = "";
#endif

  if (*value == 0)
    *len = 0;
  else
    *len = strlen (*value);

  return;
}

int
__gnat_open_read (char *path, int fmode)
{
  int fd;
  int o_fmode = O_BINARY;

  if (fmode)
    o_fmode = O_TEXT;

  fd = open (path, O_RDONLY | o_fmode, 0);

  return fd < 0 ? -1 : fd;
}

#define PERM  0

int
__gnat_open_rw (char *path, int fmode)
{
  int fd;
  int o_fmode = O_BINARY;

  if (fmode)
    o_fmode = O_TEXT;

  fd = open (path, O_RDWR | o_fmode, PERM);

  return fd < 0 ? -1 : fd;
}

int
__gnat_open_create (char *path, int fmode)
{
  int fd;
  int o_fmode = O_BINARY;

  if (fmode)
    o_fmode = O_TEXT;

  fd = open (path, O_WRONLY | O_CREAT | O_TRUNC | o_fmode, PERM);

  return fd < 0 ? -1 : fd;
}

int
__gnat_create_output_file (char *path)
{
  int fd;

  fd = open (path, O_WRONLY | O_CREAT | O_TRUNC | O_TEXT, PERM);

  return fd < 0 ? -1 : fd;
}

int
__gnat_open_append (char *path, int fmode)
{
  int fd;
  int o_fmode = O_BINARY;

  if (fmode)
    o_fmode = O_TEXT;

  fd = open (path, O_WRONLY | O_CREAT | O_APPEND | o_fmode, PERM);

  return fd < 0 ? -1 : fd;
}

/*  Open a new file.  Return error (-1) if the file already exists.  */

int
__gnat_open_new (char *path, int fmode)
{
  int fd;
  int o_fmode = O_BINARY;

  if (fmode)
    o_fmode = O_TEXT;

  fd = open (path, O_WRONLY | O_CREAT | O_EXCL | o_fmode, PERM);

  return fd < 0 ? -1 : fd;
}

/* Open a new temp file.  Return error (-1) if the file already exists.
   Special options for VMS allow the file to be shared between parent and child
   processes, however they really slow down output.  Used in gnatchop.  */

/*int
__gnat_open_new_temp (char *path, int fmode)
{
} */

/* Return the number of bytes in the specified file.  */
/*
long
__gnat_file_length (int fd)
{
}
*/
/* Return the number of bytes in the specified named file.  */

/*long
__gnat_named_file_length (char *name)
{
  int ret;
  struct stat statbuf;

  ret = __gnat_stat (name, &statbuf);
  if (ret || !S_ISREG (statbuf.st_mode))
    return 0;

  return (statbuf.st_size);
}*/

/* Create a temporary filename and put it in string pointed to by
   TMP_FILENAME.  */
/*
void
__gnat_tmp_name (char *tmp_filename)
{
}*/

/* Read the next entry in a directory.  The returned string points somewhere
   in the buffer.  */

/*
char *
__gnat_readdir (DIR *dirp, char *buffer)
{
}*/

/* Returns 1 if readdir is thread safe, 0 otherwise.  */

/*
int
__gnat_readdir_is_thread_safe (void)
{
}*/

/* Return a GNAT time stamp given a file name.  */

/*OS_Time
__gnat_file_time_name (char *name)
{

  struct stat statbuf;
  if (__gnat_stat (name, &statbuf) != 0) {
     return (OS_Time)-1;
  } else {
     return (OS_Time)statbuf.st_mtime;
  }
}*/

/* Return a GNAT time stamp given a file descriptor.  */

/*
OS_Time
__gnat_file_time_fd (int fd)
{
}
*/

/* Set the file time stamp.  */

/*
void
__gnat_set_file_time_name (char *name, time_t time_stamp)
{
}*/

/*
void
__gnat_get_env_value_ptr (char *name, int *len, char **value)
{
  *value = getenv (name);
  if (!*value)
    *len = 0;
  else
    *len = strlen (*value);

  return;
}
*/

/*void
__gnat_set_env_value (char *name, char *value)
{
}
*/


/*char *
__gnat_get_libraries_from_registry (void)
{
  char *result = (char *) "";

  return result;
}*/

/*
int
__gnat_stat (char *name, struct stat *statbuf)
{
  return stat (name, statbuf);
}
*/

/*int
__gnat_file_exists (char *name)
{
  struct stat statbuf;

  return !__gnat_stat (name, &statbuf);
}*/

/*
int
__gnat_is_absolute_path (char *name, int length)
{
  return (length != 0) &&
     (*name == '/' || *name == DIR_SEPARATOR
	  );
}*/

/*int
__gnat_is_regular_file (char *name)
{
  int ret;
  struct stat statbuf;

  ret = __gnat_stat (name, &statbuf);
  return (!ret && S_ISREG (statbuf.st_mode));
}*/

/*int
__gnat_is_directory (char *name)
{
  int ret;
  struct stat statbuf;

  ret = __gnat_stat (name, &statbuf);
  return (!ret && S_ISDIR (statbuf.st_mode));
}*/

/*
int
__gnat_is_readable_file (char *name)
{
  int ret;
  int mode;
  struct stat statbuf;

  ret = __gnat_stat (name, &statbuf);
  mode = statbuf.st_mode & S_IRUSR;
  return (!ret && mode);
}*/

/*
int
__gnat_is_writable_file (char *name)
{
  int ret;
  int mode;
  struct stat statbuf;

  ret = __gnat_stat (name, &statbuf);
  mode = statbuf.st_mode & S_IWUSR;
  return (!ret && mode);
}*/

/*
void
__gnat_set_writable (char *name)
{
  struct stat statbuf;

  if (stat (name, &statbuf) == 0)
  {
    statbuf.st_mode = statbuf.st_mode | S_IWUSR;
    chmod (name, statbuf.st_mode);
  }
}*/

/*void
__gnat_set_executable (char *name)
{
  struct stat statbuf;

  if (stat (name, &statbuf) == 0)
  {
    statbuf.st_mode = statbuf.st_mode | S_IXUSR;
    chmod (name, statbuf.st_mode);
  }
}*/

/*
void
__gnat_set_readonly (char *name)
{
#ifndef __vxworks
  struct stat statbuf;

  if (stat (name, &statbuf) == 0)
  {
    statbuf.st_mode = statbuf.st_mode & 07577;
    chmod (name, statbuf.st_mode);
  }
#endif
}
*/

/*
int
__gnat_is_symbolic_link (char *name )
{
  return 0;
}*/

/*
int
__gnat_portable_spawn (char *args[])
{
  
}*/

/* Create a copy of the given file descriptor.
   Return -1 if an error occurred.  */

/*
int
__gnat_dup (int oldfd)
{
}*/

/* Make newfd be the copy of oldfd, closing newfd first if necessary.
   Return -1 if an error occured.  */

/*int
__gnat_dup2 (int oldfd, int newfd)
{

}
*/


/*int
__gnat_portable_no_block_spawn (char *args[])
{

}*/

/*int
__gnat_portable_wait (int *process_status)
{
}*/

void
__gnat_os_exit (int status)
{
  exit (status);
}

/* Locate a regular file, give a Path value.  */

/*char *
__gnat_locate_regular_file (char *file_name, char *path_val)
{
}*/

/* Locate an executable given a Path argument. This routine is only used by
   gnatbl and should not be used otherwise.  Use locate_exec_on_path
   instead.  */
/*
char *
__gnat_locate_exec (char *exec_name, char *path_val)
{
  if (!strstr (exec_name, HOST_EXECUTABLE_SUFFIX))
    {
      char *full_exec_name
        = alloca (strlen (exec_name) + strlen (HOST_EXECUTABLE_SUFFIX) + 1);

      strcpy (full_exec_name, exec_name);
      strcat (full_exec_name, HOST_EXECUTABLE_SUFFIX);
      return __gnat_locate_regular_file (full_exec_name, path_val);
    }
  else
    return __gnat_locate_regular_file (exec_name, path_val);
}*/

/* Locate an executable using the Systems default PATH.  */

/*char *
__gnat_locate_exec_on_path (char *exec_name)
{
}*/


/* Dummy functions for Osint import for non-VMS systems.  */

int
__gnat_to_canonical_file_list_init
  (char *dirspec , int onlydirs )
{
  return 0;
}

char *
__gnat_to_canonical_file_list_next (void)
{
  return (char *) "";
}

void
__gnat_to_canonical_file_list_free (void)
{
}

char *
__gnat_to_canonical_dir_spec (char *dirspec, int prefixflag )
{
  return dirspec;
}

char *
__gnat_to_canonical_file_spec (char *filespec)
{
  return filespec;
}

char *
__gnat_to_canonical_path_spec (char *pathspec)
{
  return pathspec;
}

char *
__gnat_to_host_dir_spec (char *dirspec, int prefixflag )
{
  return dirspec;
}

char *
__gnat_to_host_file_spec (char *filespec)
{
  return filespec;
}

void
__gnat_adjust_os_resource_limits (void)
{
}


/* Dummy function to satisfy g-trasym.o.  Currently Solaris sparc, HP/UX,
   GNU/Linux x86{_64}, Tru64 & Windows provide a non-dummy version of this
   procedure in libaddr2line.a.  */

void
convert_addresses (void *addrs ,
		   int n_addr ,
		   void *buf ,
		   int *len )
{
  *len = 0;
}

int __gnat_argument_needs_quote = 0;

/* This option is used to enable/disable object files handling from the
   binder file by the GNAT Project module. For example, this is disabled on
   Windows (prior to GCC 3.4) as it is already done by the mdll module.
   Stating with GCC 3.4 the shared libraries are not based on mdll
   anymore as it uses the GCC's -shared option  */
#if defined (_WIN32) \
    && ((__GNUC__ < 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ < 4)))
int __gnat_prj_add_obj_files = 0;
#else
int __gnat_prj_add_obj_files = 1;
#endif

/* char used as prefix/suffix for environment variables */
char __gnat_environment_char = '$';

/* This functions copy the file attributes from a source file to a
   destination file.

   mode = 0  : In this mode copy only the file time stamps (last access and
               last modification time stamps).

   mode = 1  : In this mode, time stamps and read/write/execute attributes are
               copied.

   Returns 0 if operation was successful and -1 in case of error. */

/*
int
__gnat_copy_attribs (char *from, char *to, int mode)
{
  
}
*/

/* This function is installed in libgcc.a.  */
extern void __gnat_install_locks (void (*) (void), void (*) (void));

/* This function offers a hook for libgnarl to set the
   locking subprograms for libgcc_eh.
   This is only needed on OpenVMS, since other platforms use standard
   --enable-threads=posix option, or similar.  */

void
__gnatlib_install_locks (void (*lock) (void) ,
                         void (*unlock) (void) )
{
}

int
__gnat_lseek (int fd, long offset, int whence)
{
  //return (int) lseek (fd, offset, whence);
  return 0;
}

/* This function returns the version of GCC being used.  Here it's GCC 3.  */
int
get_gcc_version (void)
{
  return 3;
}

int
__gnat_set_close_on_exec (int fd,
                        int close_on_exec_p)
{
  return -1;

}
