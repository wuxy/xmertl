/*
 * $FILE: libgen.c
 *
 * Definitions for pattern matching functions
 *
 * $VERSION: 1.0
 *
 * Author: Miguel Masmano <mimastel@doctor.upv.es>
 *
  * $LICENSE:
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stddef.h>
#include <string.h>
#include <libgen.h>

char *basename(char *path) {
  char *c;
 again:
  if (!(c=strrchr(path,'/'))) return path;
  if (c[1]==0) {
    if (c == path)
      return c;
    else {
      *c=0;
      goto again;
    }
  }
  return c+1;
}

static char *dot=".";
#define SLASH '/'
#define EOL (char)0
char *dirname(char *path)
{
  char *c;
  if ( path  == NULL ) return dot;
  for(;;) {
    if ( !(c=strrchr(path,SLASH)) ) return dot;
    if ( c[1]==EOL && c!=path ) {
      while ( *c==SLASH && c!=path ) *c--=EOL;
      continue;
    }
    if ( c!=path )
      while ( *c==SLASH ) *c--=EOL;
    else
      path[1]=EOL;
    return path;
  }
}
#undef SLASH
#undef EOL
