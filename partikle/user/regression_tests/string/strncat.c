#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../regression.h"

int main() {
  char buf[100]="fnord";
  strncat(buf,"foo",0);
  if (strcmp(buf,"fnord")) 
    eprintf("strncat did not work for length 0");
  strncat(buf,"foo",2);
  if (strcmp(buf,"fnordfo")) 
    eprintf ("strncat did not copy n bytes");
  return TEST_OK;
}
