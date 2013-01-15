// interface functions for console i/o,
// these are defined for each arch and are implemented
// by arch dependend function calls.

extern int read_cons (int fd, void * str, long n);
extern int write_cons (int fd, const void * str, long n);
