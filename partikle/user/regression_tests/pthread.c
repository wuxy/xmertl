/*
 * $FILE: pthread.c
 *
 * Pthread regression tests
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

#include <pthread.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "regression.h"

#define NTHREADS 50

static void *thread_body_1 (void *arg) {
  printf ("Soy %d\n", (int) arg);
  return 0;
}

static int create_pthread (void) {
  pthread_t t [NTHREADS];
  int n;

  for (n = 0; n < NTHREADS; n++)
    pthread_create (&t [n], NULL, thread_body_1, (void *) n);
  
  for (n = 0; n < NTHREADS; n++)
    pthread_join (t[n], NULL);

  return 0;
}


int main (int argc, char * argv[]) {
  if (create_pthread ())
    return TEST_FAIL;
  return TEST_OK;
}
