#ifndef ABT_H
#define ABT_H

#ifdef __cplusplus
#include <cstddef>
extern "C" {
#else
#include <stddef.h>
#include <stdbool.h>
#endif

// in test
void abt_start(void);
void abt_end(void);
bool abt_is_successful_branch(void);

// in mock
bool abt_started(void);
size_t abt_branch(size_t count);
void abt_error(const char *const msg /* static */); // noreturn
void abt_fail(const char *const msg /* static */); // noreturn
void abt_mark_as_failure(void);
void abt_pause(void);
void abt_resume(void);

#ifdef __cplusplus
}
#endif

#endif
