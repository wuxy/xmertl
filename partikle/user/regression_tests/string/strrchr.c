#include <assert.h>
#include <string.h>
#include "../regression.h"

int main() {
  const char* x="foo bar baz";
  assert(strrchr(x,'z')==x+10);
  assert(strrchr(x,' ')==x+7);
  assert(strrchr(x,'x')==0);
  return TEST_OK;
}
