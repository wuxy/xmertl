#include <xm_syscalls.h>

void handler (int event, void *a) 
{
	write_scr("HELLO\n", 6);
}

int kmain (void) 
{
	struct xmitimerval req = {{0, 200000}, {0, 0}};
	
	install_event_handler (0, handler);
  	set_timer (&req, 0);
	unmask_event (0);
  
	enable_events_flag();
	while (1) {
		suspend_domain (0,0);
	}

	return 0;
}
