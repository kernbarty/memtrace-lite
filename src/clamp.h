#ifndef CLAMP_H
#define CLAMP_H

#include <stdint.h>
#include <stddef.h>

/*
 * clamp.h - Value clamping utilities for memory metrics
 *
 * Provides min/max boundary enforcement for sampled memory
 * values, preventing outliers from skewing analysis.
 */

typedef struct {
    uint64_t min;       /* lower bound (inclusive) */
    uint64_t max;       /* upper bound (inclusive) */
    uint64_t clamped;   /* number of values clamped so far */
    uint64_t total;     /* total values processed */
} clamp_t;

/* Initialize a clamp with given bounds */
void clamp_init(clamp_t *c, uint64_t min, uint64_t max);

/* Apply clamping to a value; updates internal counters */
uint64_t clamp_apply(clamp_t *c, uint64_t value);

/* Returns ratio of clamped values to total (0.0 - 1.0) */
double clamp_ratio(const clamp_t *c);

/* Reset counters without changing bounds */
void clamp_reset(clamp_t *c);

/* Update bounds dynamically */
void clamp_set_bounds(clamp_t *c, uint64_t min, uint64_t max);

#endif /* CLAMP_H */
