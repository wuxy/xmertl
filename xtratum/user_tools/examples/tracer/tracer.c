#include <xm_syscalls.h>

void handler (int event, void *a) {
  //write_scr ("irq 0\n", 6);
  write_scr("hola\n", 6);
     //pass_event (event);
     //unmask_event (0);
}

int kmain (void) {
  struct xmitimerval req = {{0, 200000}, {0, 0}};
  struct xmtimespec t;
  int i;

  install_event_handler (0, handler);
  
  set_timer (&req, 0);

  unmask_event (0);

  get_time(&t);

  xmtrace_user(20,4711);
  xmtrace_user(18,4711);

  //xprint("nsec %lu sec %lu\n",t.tv_nsec, t.tv_sec);
  
  enable_events_flag();
  while (1){
	  for(i=0;i< 100; i++){
			  
  		xmtrace_user(20,i);  //RTL_TRACE_USER2
  		xmtrace_user(18,0);  //RTL_TRACE_FINALIZE
	  }
  suspend_domain (0,0);
  }
  return 0;
}
