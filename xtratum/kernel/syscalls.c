/*
 * $FILE: syscalls.c
 *
 * XM's syscalls
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
#include <syscalls.h>
#include <linux_inc.h>
#include <errno.h>
#include <mm.h>
#include <xmtrace.h>

#include <arch/cpu.h>

#include <log.h>
#include <xmdev.h>

//-----------------//
// exit_domain_sys //
//-----------------//

static void exit_domain_sys (int val) {
  disable_events_flag (xm_current_domain);
  set_domain_state(xm_current_domain, DOMAIN_FINISHED);
//  remove_domain_list (xm_current_domain, &xm_domain_list);

  xm_sched ();
  // Never returns
}

//-----------------//
// load_domain_sys //
//-----------------//

static int load_domain_sys (char *dname, int prio, domain_image_t *dimg) {
  domain_t *d = 0;
  unsigned long flags, addr;
  /*, s_event_stack_code;*/
  int nsec, id, trap;
#ifdef XM_DEV_SUPPORT
  unsigned long fvaddr;
  xm_dev_t *dev;
  int findex;
#endif

  if (xm_current_domain != xm_root_domain) return -ONLY_ROOT_DOMAIN_CALL;

  if (prio > min_priority () || prio < max_priority ()) return -BAD_PRIORITY;

  hw_save_flags_and_cli (&flags);

  d = create_domain (dname, prio, 0);

  id = d -> id;
  if (!d) {
    id = -OUT_OF_MEMORY;
    goto exit_load_domain;
  }

  set_domain_state(d, DOMAIN_ACTIVE);
  
  d -> pd = create_page_directory (mm_alloc_page);
  
  if (!d -> pd) {
    destroy_domain (d);
    id = -OUT_OF_MEMORY;
    goto exit_load_domain;
  }
  
  // Creating a virtual memory map with the domain's image
  for (nsec = 0; nsec < dimg -> nsec; nsec++) {
    unsigned long addr, start, end, pg_addr, img_addr, offset;
    int lsize, csize;

    img_addr = (unsigned long)dimg -> section [nsec].img;
    start = (dimg -> section [nsec].st_addr & PAGE_MASK);
    end = dimg -> section [nsec].st_addr + dimg -> section [nsec].size;
    end = !(end & ~PAGE_MASK)? end: (end & PAGE_MASK) + PAGE_SIZE;
    
    offset = (dimg -> section [nsec].st_addr & (PAGE_SIZE - 1));
    lsize = dimg -> section [nsec].size;
    csize = ((PAGE_SIZE - offset) < lsize)? (PAGE_SIZE - offset): lsize;
    for (addr = start; addr < end; addr += PAGE_SIZE) {
      if (!(pg_addr = allocate_user_page (d->pd, addr, mm_alloc_page))) {
	destroy_domain (d);
	id = -OUT_OF_MEMORY;
	goto exit_load_domain;
      }
#if 0
      printk ("copying: 0x%x to 0x%x (%d bytes)\n", img_addr, pg_addr
	      + offset, csize);
#endif
      memcpy ((unsigned char *)(pg_addr + offset), (unsigned char *)img_addr, 
	      csize);
      offset = 0;
      img_addr += csize;
      lsize -= csize;
      csize = (PAGE_SIZE < lsize)? PAGE_SIZE: lsize;
    }
  }
//  id = -1;
//   destroy_domain (d);
//    goto exit_load_domain; 
  // Creating a valid stack
  // Firstly we have to find a valid range of free memory
  
  /*d->ustack_st = (unsigned long *)alloc_free_user_range 
    (d -> pd, ((unsigned long)DEFAULT_USTACK_ADDR),
     ((unsigned long)DEFAULT_USTACK_SIZE), mm_alloc_page);

  if (!d->ustack_st) {
    destroy_domain (d);
    id = -OUT_OF_MEMORY;
    goto exit_load_domain;
  }

  d->ustack = (unsigned long *)((unsigned long)d->ustack_st +
				DEFAULT_USTACK_SIZE-sizeof(unsigned long));
  */
  d->sstack_st = vmalloc (DEFAULT_SSTACK_SIZE);

  if (!d->sstack_st) {
    destroy_domain (d);
    id = -OUT_OF_MEMORY;
    goto exit_load_domain;
  }

  d->sstack = (unsigned long *)
    ((unsigned long)d->sstack_st + DEFAULT_SSTACK_SIZE);
    
  init_sstack (d->sstack, dimg->entry_point, init_wrapper);

  // Creating an events page
  /*for (addr = (unsigned long)d->ustack + 
	 sizeof (unsigned long) + (PAGE_SIZE * 4);  
	 addr < PAGE_OFFSET; addr += PAGE_SIZE)*/
  
  for (addr = DEFAULT_EVENT_ADDR;
       addr < PAGE_OFFSET; addr += PAGE_SIZE)
    if ((d->events = (event_handling_t *)allocate_user_page 
	 (d -> pd, addr, mm_alloc_page)) > 0) break;

  if (addr >= PAGE_OFFSET) {
    destroy_domain (d);
    id = -OUT_OF_MEMORY;
    goto exit_load_domain;
  }
  
  d-> events_user_addr = addr;

  // When an irq is triggered, XM brings up the domain's stack to
  // handle the event, once the event has been process, the domain has
  // to restore its stack, the code to carry out this action is
  // located after its own event's structure
  
  /*_restore_event_stack_code = addr + sizeof (event_handling_t);
  s_event_stack_code = ((unsigned long)d -> events) + 
    sizeof (event_handling_t);
  *((unsigned long *)s_event_stack_code) = RESTORE_EVENT_STACK ();
  */
  disable_events_flag (d);

  for (trap = 0; trap < MAX_TRAPS; trap ++)
    d -> events -> trap_handler [trap] = generic_trap_handler;

  // Creating a heap area
  // Firstly we have to find a valid range of free memory
  
  d->events->heap = d->heap = (unsigned long *)alloc_free_user_range 
    (d -> pd, ((unsigned long)d-> events_user_addr + PAGE_SIZE * 4),
     DEFAULT_HEAP_SIZE, mm_alloc_page);
  
  d->events->heap_size = DEFAULT_HEAP_SIZE;
  
  if (!d->heap) {
    destroy_domain (d);
    id = -OUT_OF_MEMORY;
    goto exit_load_domain;
  }

#if XM_FIFO_DEV_SUPPORT
	/* xm fifo device set */  
	fvaddr = DEFAULT_FIFO_ADDR;

	if ((dev = xm_get_dev(XM_FIFO_DEV)) == NULL) {
		destroy_domain (d);
		id = -ENODEV;
		goto exit_load_domain;
	}
  
	if (dev->dev_map_handler(d->pd, fvaddr, 
				(XM_FIFO_COUNT+1)*PAGE_SIZE, NULL)) { 
		destroy_domain (d);
		id = -EINVAL;
		goto exit_load_domain;
	}
  
	d->events->fctladdr = fvaddr;

	for(findex = 0; findex < XM_FIFO_COUNT; findex ++) {
		fvaddr += PAGE_SIZE;
		d->events->fdataddr[findex] = fvaddr;
	}

	dev->client ++;
#endif

#if XM_SHM_DEV_SUPPORT

	/*xm share memory support */
#endif

#if XM_SERIAL_DEV_SUPPORT
	fvaddr = DEFAULT_SERIAL_ADDR;

	if ((dev = xm_get_dev(XM_SERIAL_DEV)) == NULL) {
		destroy_domain (d);
		id = -ENODEV;
		goto exit_load_domain;
	}
	
	if ( is_serial_dev( d ) ) {
		dev->dev_map_handler(d->pd, fvaddr, XM_SERIAL_COUNT*2*PAGE_SIZE, NULL);
		d->events->serialaddr = fvaddr;
	} 
	
	dev->client ++;
#endif

exit_load_domain:

  hw_restore_flags (flags);

  return id;
}

//-------------------//
// unload_domain_sys //
//-------------------//

static int unload_domain_sys (int id) {
  unsigned long flags, err = 0;
  xm_dev_t *dev;
  domain_t *d;

  if (xm_current_domain != xm_root_domain) return -ONLY_ROOT_DOMAIN_CALL;
  if (!id) return -ROOT_CANNOT_BE_UNLOADED;

  hw_save_flags_and_cli (&flags);
  d = search_domain_id_list (id, &xm_domain_list);

  if (d) {

#if XM_FIFO_DEV_SUPPORT
  	if ((dev = xm_get_dev(XM_FIFO_DEV)) != NULL) {
  		dev->dev_unmap_handler(d->pd, DEFAULT_FIFO_ADDR, (XM_FIFO_COUNT+1)*PAGE_SIZE);
		dev->client --;
  	}
#endif

#if XM_SERIAL_DEV_SUPPORT
  	if ((dev = xm_get_dev(XM_SERIAL_DEV)) != NULL) {
		if ( is_serial_dev( d ) ) {
  			dev->dev_unmap_handler(d->pd, DEFAULT_SERIAL_ADDR, XM_SERIAL_COUNT*2*PAGE_SIZE);
		} else {
  			dev->dev_unmap_handler(d->pd, DEFAULT_SERIAL_ADDR, 0);
		}
		dev->client --;
  	}
#endif

	destroy_page_directory (d -> pd, mm_release_page);
    
    	if (d->sstack_st)
      		vfree (d->sstack_st);
    	timer_remove_domain (d);
    	destroy_domain (d);
  } else err = -DOMAIN_NOT_FOUND;


  hw_restore_flags (flags);

  return err;
}

//--------------------//
// suspend_domain_sys //
//--------------------//

static void suspend_domain_sys (unsigned long dev, 
		unsigned long cmd) 
{
	int ret;

	set_domain_state (xm_current_domain, DOMAIN_SUSPENDED);

	if( cmd != 0) {
		ret = xm_dev_ioctl(dev, cmd, xm_current_domain);
		if (ret == 0) {
			set_domain_state(xm_current_domain, DOMAIN_ACTIVE);
		}
	}

	xm_sched ();
}

//-----------------//
// sync_events_sys //
//-----------------//

static void sync_events_sys (void) 
{
	sync_events ();
}

//----------------//
// pass_event_sys //
//----------------//

static void pass_event_sys (int event) {
  if (xm_current_domain -> next)
    set_bit (xm_current_domain -> next -> events -> pending_events, event);
}

//---------------//
// write_scr_sys //
//---------------//
extern volatile int xmprint;
extern int fill_printk(char *str, int length);
static int write_scr_sys (char *b, int l) {
  return fill_printk(b, l);
}

//--------------//
// enable_hwirq //
//--------------//

static int enable_hwirq_sys (int irq) {
  if (irq != hwtimer.timer_event && irq < NR_IRQS)
    hw_xpic[irq] -> end (irq);
  return 0;
}

//--------------//
// get_time_sys //
//--------------//

static int get_time_sys (struct xmtimespec *t) {
  duration_t d;
  if (t) {
    d = sysclock.gettime_nsec ();
    *t = (struct xmtimespec)
      {tv_nsec: d%NSECS_PER_SEC, tv_sec: d/NSECS_PER_SEC};
  }
  return 0;
}

//---------------//
// set_timer_sys //
//---------------//

static int set_timer_sys (struct xmitimerval *req, struct xmitimerval *rem) {
  duration_t treq_v, treq_i;
 
  timer_remove_domain (xm_current_domain);
  if (rem) {
    rem -> it_value = (struct xmtimespec)
      {tv_nsec: xm_current_domain->time.hwtime%NSECS_PER_SEC,
       tv_sec: xm_current_domain->time.hwtime/NSECS_PER_SEC}; 

    rem -> it_interval = (struct xmtimespec)
      {tv_nsec: xm_current_domain->time.period%NSECS_PER_SEC,
       tv_sec: xm_current_domain->time.period/NSECS_PER_SEC};
  }

  if (req) {
    treq_v = req -> it_value.tv_sec*NSECS_PER_SEC + 
      req -> it_value.tv_nsec;

    treq_i = req -> it_interval.tv_sec*NSECS_PER_SEC + 
      req -> it_interval.tv_nsec;

    sethwtimer (treq_v, treq_i);
  }

  return 0;
}

//-----------------//
// get_cpu_khz_sys //
//-----------------//
extern unsigned long long cpu_hz;
static unsigned long get_cpu_khz_sys (void) 
{
	return (unsigned long)(cpu_hz/1000);
}


/*
 * routine:
 *   dev_ctl_sys
 *
 * description:
 *   The routine used to control device
 *
 * parameters:
 *   dev: specify the device which is registered in the XtratuM
 *   cmd: device commands
 *   arg: cmd parameters, which will be different with kinds of cmd value
 *
 * return value:
 *   success: 0
 *   failure: not 0
 *
 * */

static int dev_ctl_sys(unsigned long dev, unsigned long cmd, void *arg)
{
	int ret = 0;

	ret = xm_dev_ioctl(dev, cmd, arg);

	return ret;
}

/* 
 * shm_open_sys
 *
 * */

static int shm_open_sys(const char *name,    
	unsigned long flag, unsigned long mode)
{
	xm_dev_t *dev;
	int minor;

	if ((dev = xm_get_dev(XM_SHM_DEV)) == NULL) {
		return -ENODEV;
	}

	minor = dev->dev_ioctl_handler(XM_SHM_COUNT, XM_SHM_CREATE, (void *)name);
	
	return minor;
}


/*
 * shm_unlink_sys
 *
 * */

static int shm_unlink_sys(const char *name)
{
	xm_dev_t *dev;
	int ret = 0, minor;              


	if ((dev = xm_get_dev(XM_SHM_DEV)) == NULL) {
		return -ENODEV;
	}

	minor = dev->dev_ioctl_handler(XM_SHM_COUNT, XM_SHM_EXIST, (void *)name);
	
	if(minor < XM_SHM_COUNT && minor >= 0)
		ret = dev->dev_ioctl_handler(minor, XM_SHM_REMOVE, (void *)name);

	return ret;
}

static void xmtrace_user_sys(int event_id, long event_data)
{
	rtl_trace2(event_id, event_data);
}


//---------------//
// syscall_table //
//---------------//

syscall_handler_t 
syscall_table [SYSCALL_ENTRIES] = {
  exit_domain_sys,
  load_domain_sys,
  unload_domain_sys,
  suspend_domain_sys,
  sync_events_sys,
  pass_event_sys,
  get_time_sys,
  set_timer_sys,
  get_cpu_khz_sys,
  write_scr_sys,
  enable_hwirq_sys,
  dev_ctl_sys,
  shm_open_sys,
  shm_unlink_sys,
  xmtrace_user_sys
};
