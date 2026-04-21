#ifndef FENCE_H
#define FENCE_H

#include <stddef.h>
#include <stdint.h>

/*
 * fence — memory usage boundary enforcement
 *
 * A fence defines a hard upper and soft warning boundary for memory
 * consumption. It tracks how many times each boundary has been crossed
 * and provides a simple query interface.
 */

typedef enum {
    FENCE_OK      = 0,  /* usage is within soft boundary */
    FENCE_WARN    = 1,  /* usage exceeded soft boundary */
    FENCE_BREACH  = 2   /* usage exceeded hard boundary */
} fence_status_t;

typedef struct {
    size_t   soft_kb;        /* warning threshold in KiB */
    size_t   hard_kb;        /* hard limit in KiB */
    uint64_t warn_count;     /* number of soft crossings */
    uint64_t breach_count;   /* number of hard crossings */
    size_t   peak_kb;        /* highest observed usage */
} fence_t;

/* Initialise a fence with soft and hard limits (in KiB). */
void fence_init(fence_t *f, size_t soft_kb, size_t hard_kb);

/* Evaluate current usage against the fence boundaries.
 * Updates internal counters and peak. Returns the status. */
fence_status_t fence_check(fence_t *f, size_t current_kb);

/* Return a human-readable label for a fence_status_t value. */
const char *fence_status_str(fence_status_t status);

/* Reset counters (warn_count, breach_count, peak) without changing limits. */
void fence_reset(fence_t *f);

#endif /* FENCE_H */
