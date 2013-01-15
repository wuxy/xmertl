/*
 * $FILE: domain.c
 *
 * Domains' routines
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

#include <domain.h>
#include <sched.h>

domain_t *xm_current_domain = NULL, *xm_root_domain = NULL, 
  *xm_domain_list = NULL;

EXPORT_SYMBOL(xm_current_domain);

static int id_number = 0;

//----------------------//
// __create_domain_node //
//----------------------//

domain_t *__create_domain_node (char *name, unsigned long priority) {
  domain_t *d;
 
  if (!(d = (domain_t *) vmalloc (sizeof (domain_t)))) {
    printk ("create_domain (): Error calling vmalloc\n");
    printk ("(Out of memory?)\n");
    return NULL;
  }
 
  memset ((char *)d, 0, sizeof (domain_t));
  
  if (!(d -> name = (char *) vmalloc 
	  (sizeof (char) * (strlen (name) + 1)))) {
    printk ("create_domain (): Error calling vmalloc\n");
    printk ("(Out of memory?)\n");
    vfree (d);
    return NULL;
  }
 
  if (name)
    strcpy (d -> name, name);
  
  d -> priority = priority;
  d -> id = id_number ++;

  return d;
}

//----------------------//
// __remove_domain_node //
//----------------------//

void __remove_domain_node (domain_t *domain) {
  if (domain && domain -> name)
    vfree (domain -> name);
  if (domain)
    vfree (domain);
}

//---------------//
// create_domain //
//---------------//

domain_t *create_domain (char *name, unsigned long priority,
			 int (*main_func) (void)) {
  domain_t *d = __create_domain_node (name, priority);

  insert_domain_list (d, &xm_domain_list);
  
  return d;
}

//----------------//
// destroy_domain //
//----------------//

void destroy_domain (domain_t *d) {
  remove_domain_list (d, &xm_domain_list);
  __remove_domain_node (d);
}
