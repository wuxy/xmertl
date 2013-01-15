To keep partikle small Java is packaged in a separate module.

To enable java support checkout the branch under branches/partikle-java,
running the following command, under this directory:
% svn co https://www.gii.upv.es/svn/rtos/branches/partikle-java java

This will create the java directory which will be processed by the Makefile.

issue:
Since partikle syscall sched_yield was changed to pthread_yield
and libgcj config relies on HAVE_SCHED_YIELD under include/libgcj_config.h,
the places where this function is needed should be corrected.
