// A Hello World example

#include <stdio.h>
#include <time.h>
#include <sys/io.h>
#include <rtl.h>

int main (int argc, char **argv) {
  struct timespec t = {5, 0};
  float a, b;
  a = 6.0;
  b = 7.0;
  a = a*b;
  a = a/b;
  //  int ent;
//  char buf [20];
  printf ("Hello World!!! %f\n", a);
/*  scanf ("%d", &ent);
  scanf ("%s", buf);
  printf ("ent: %d\n", ent);
  printf ("buf: %s\n", buf);
  ent = inb (0x378); */
//  while (1);
  nanosleep (&t, 0);
  printf ("Bye bye!!! %llu\n",clock_gethrtime(CLOCK_REALTIME));
//  exit (0);
  return 0;
}
