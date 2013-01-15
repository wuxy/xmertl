#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../regression.h"

int main() {
  char buf[100]="fnord";
  char buf2[100]="fnort";
  if (strcmp(buf,buf))
	  eprintf("strcmp say a != a");
  if (strcmp(buf,buf2)>=0)
	  eprintf("strcmp said fnord > fnort");
  if (strcmp(buf2,buf)<=0) 
	  eprintf("strcmp said fnort < fnord");
  return TEST_OK;
}
