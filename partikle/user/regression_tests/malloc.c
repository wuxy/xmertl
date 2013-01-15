// Creating our first thread

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "regression.h"

int main (int argc, char **argv) {
  int *ptr[100];
  int i;

  for (i=0; i< 100; i++)
    if (!(ptr[i]=malloc (1024))){
      printf("Error\n");
      return TEST_FAIL;
    }
  for (i=0; i< 100; i++)
    free(ptr[i]);

  printf("Test OK\n");
  return TEST_OK;
}
