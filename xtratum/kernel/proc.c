/*
 * $FILE: proc.c
 *
 * Shows information about XM
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

#include <linux_inc.h>
#include <domain.h>
#include <sched.h>

extern unsigned long xm_n_free_pages;

static int proc_calc_metrics(char *page, char **start, off_t off,
                                 int count, int *eof, int len) {
  if (len <= off+count) *eof = 1;
  *start = page + off;
  len -= off;
  if (len>count) len = count;
  if (len<0) len = 0;
  return len;
}

static int xm_read_proc (char *page, char **start, off_t off,
			  int count, int *eof, void *data) {
  domain_t *sel_domain;
  int len = 0;//, i;
  char *ptr;
  unsigned long flags;
  
  hw_save_flags_and_cli (&flags);
    
  ptr = page;
  ptr += sprintf (ptr, "XtratuM domains:\n");
  ptr += sprintf (ptr, "----------------\n\n");
  
  for (sel_domain = xm_domain_list; sel_domain;
       sel_domain = sel_domain -> next) {
    ptr += sprintf (ptr, "(%d) %s:\n", sel_domain -> id, 
		    sel_domain -> name);
    ptr += sprintf (ptr, "\tPriority: %u ", sel_domain -> priority);
    if (sel_domain -> priority == max_priority())   
      ptr += sprintf (ptr, "(MAX. PRIORITY)");
    if (sel_domain -> priority == min_priority())   
      ptr += sprintf (ptr, "(MIN. PRIORITY)");
    ptr += sprintf (ptr, "\n");
    ptr += sprintf (ptr, "\tIntercepted events: 0x%x\n ", 
		    (unsigned int) sel_domain -> events -> intercepted_events);
    ptr += sprintf (ptr, "\tMasked events: 0x%x\n ", 
		    (unsigned int) sel_domain -> events -> masked_events);
    ptr += sprintf (ptr, "\tPending events: 0x%x\n ", 
		    (unsigned int) sel_domain -> events -> pending_events);
    if (is_events_flag_enabled(sel_domain))
      ptr += sprintf (ptr, "\tEvents' flag is enabled\n");    
    else
      ptr += sprintf (ptr, "\tEvents' flag is disabled\n");

    ptr += sprintf (ptr, "\tCurrent state: ");
    switch (get_domain_state (sel_domain)) {
    case DOMAIN_ACTIVE:
      ptr += sprintf (ptr, "ACTIVE\n");
      break;
    case DOMAIN_FINISHED:
      ptr += sprintf (ptr, "FINISHED\n");
      break;
    case DOMAIN_SUSPENDED:
      ptr += sprintf (ptr, "SUSPENDED\n");
      break;
    default:
      ptr += sprintf (ptr, "Unknown state (ERROR)\n");
    }
    ptr += sprintf (ptr, "\tDomain's state word: 0x%x\n", 
		    (int) sel_domain -> flags);
    ptr += sprintf (ptr, "\n");
  }
  len = strlen (page);
  hw_restore_flags (flags);

  return proc_calc_metrics (page, start, off, count, eof, len);
}


#define PROC_ENTRY_NAME "xtratum"
void create_xm_proc (void) {
  //return 
  create_proc_read_entry
    (PROC_ENTRY_NAME, 0, NULL, xm_read_proc, NULL);
} 

void remove_xm_proc (void) {
  remove_proc_entry (PROC_ENTRY_NAME, NULL);
}
