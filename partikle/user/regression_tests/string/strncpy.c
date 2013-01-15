#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../regression.h"

int main() {
  char buf[100];
  assert(strncpy(buf,"fnord",6)==buf);
  assert(!strcmp(buf,"fnord"));
  memset(buf,23,sizeof buf);
  assert(strncpy(buf,"fnord",5)==buf);
  printf("%c%c%c%c%c %d %d\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[5], buf[6]);
  assert(!memcmp(buf,"fnord",5) && buf[5]==23);
  return TEST_OK;
}
