#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/abt.h"
#include "malloc_mock.h"

typedef bool err_t;

err_t testee(void **out_a, void **out_b) {
  void *const a = malloc(10);
  if (!a) {
    return true;
  }
  void *const b = malloc(10);
  if (!b) {
    return true;
  }
  *out_a = a;
  *out_b = b;
  return false;
}

void tester(void) {
  abt_start();
  void *a;
  void *b;
  const err_t result = testee(&a, &b);
  if (!result && !abt_is_successful_branch()) {
    abt_fail("At least one allocation failed but result is OK");
  }
  if (result && abt_is_successful_branch()) {
    abt_fail("All allocation succeed but result is not OK");
  }
  if (!result) {
    free(a);
    free(b);
  }
  if (malloc_mock_get_counter()) {
    abt_fail("Found leaked memory");
  }
  abt_end();
  fprintf(stderr, "This will not be printed\n");
}

int main(void) {
  tester();
  return 0;
}
