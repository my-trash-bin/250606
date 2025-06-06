#define _GNU_SOURCE

#include "malloc_mock.h"

#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/abt.h"

// allocated my_resource count
static size_t counter = 0;

void *malloc(size_t size) {
  static void *(*real)(size_t size) = NULL;

  if (!real)
    real = (void *(*)(size_t size))dlsym(RTLD_NEXT, "malloc");

  // return real result if not testing
  if (!abt_started()) {
    return real(size);
  }

  // we will test two cases
  size_t branch = abt_branch(2);

  switch (branch) {
  case 0: {
    // case 0: simulate allocation failure
    abt_mark_as_failure();
    return NULL;
  }
  case 1: {
    // case 1: simulate allocation success
    void *const result = real(size);
    if (!result) {
      // allocation failure during test == test error
      abt_error("init_my_resource() failed");
    }
    if (result) {
      counter++;
    }
    return result;
  }
  default:
    abort();
  }
}

void free(void *ptr) {
  static void (*real)(void *ptr) = NULL;

  if (!real)
    real = (void (*)(void *ptr))dlsym(RTLD_NEXT, "free");

  if (!abt_started()) {
    real(ptr);
    return;
  }

  if (ptr) {
    counter--;
  }
  real(ptr);
}

size_t malloc_mock_get_counter(void) { return counter; }
