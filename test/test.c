#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/abt.h"
#include "malloc_mock.h"

typedef bool err_t;

err_t testee_p(void **out_a, void **out_b) {
  void *const a = malloc(10);
  if (!a) {
    return true;
  }
  void *const b = malloc(10);
  if (!b) {
    free(a);
    return true;
  }
  *out_a = a;
  *out_b = b;
  return false;
}

void tester_p(void) {
  abt_start();
  void *a;
  void *b;
  const err_t result = testee_p(&a, &b);
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
    abt_fail("UNEXPECTED: Found leaked memory");
  }
  fprintf(stderr, "This will be printed three times\n");
  abt_end();
  fprintf(stderr, "This will be printed once\n");
}

err_t testee_f(void **out_a, void **out_b) {
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

void tester_f(void) {
  abt_start();
  void *a;
  void *b;
  const err_t result = testee_f(&a, &b);
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
    abt_fail("EXPECTED: Found leaked memory");
  }
  abt_end();
  fprintf(stderr, "This will not be printed\n");
}

int main(void) {
  tester_p();
  tester_f();
  return 0;
}
