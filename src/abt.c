// It will not work on Windows even on Cygwin, as it uses RTLD_NEXT.

#include "../include/abt.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static bool started = false;
static bool failure = false;
static bool is_forked = false;
static size_t pause_count = 0;

void abt_start(void) {
  if (started) {
    abt_error("abt_start() called twice");
  }
  started = true;
  failure = false;
}

void abt_end(void) {
  if (!started) {
    abt_error("abt_end() called without abt_start()");
  }
  if (pause_count) {
    abt_error("abt_end() called with abt_pause()");
  }
  started = false;
  if (is_forked) {
    exit(0);
  } else {
    int status;
    while (wait(&status) > 0) {
      if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        exit(1); // Propagate failure from child
      }
    }
  }
}

bool abt_is_successful_branch(void) { return !failure; }

bool abt_started(void) { return started; }

size_t abt_branch(size_t count) {
  for (size_t i = 1; i < count; i++) {
    pid_t pid = fork();
    if (pid < 0) {
      abt_error("Internal error: fork() failed");
    } else if (pid == 0) {
      // Child process
      is_forked = true;
      return i;
    }
  }
  return 0;
}

void abt_error(const char *const msg) {
  fprintf(stderr, "ABT Error: %s\n", msg);
  exit(1);
}

void abt_fail(const char *const msg) {
  fprintf(stderr, "ABT Failure: %s\n", msg);
  exit(1);
}

void abt_mark_as_failure(void) {
  if (!started) {
    abt_error("abt_mark_as_failure() called without abt_start()");
  }
  failure = true;
}

void abt_pause(void) {
  if (!started) {
    abt_error("abt_pause() called without abt_start()");
  }
  pause_count++;
}

void abt_resume(void) {
  if (!started) {
    abt_error("abt_resume() called without abt_start()");
  }
  if (pause_count == 0) {
    abt_error("abt_resume() called without abt_pause()");
  }
  pause_count--;
}
