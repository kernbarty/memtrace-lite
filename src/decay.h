#ifndef DECAY_H
#define DECAY_H

#include <stddef.h>
#include <time.h>

/* Exponential decay smoother for memory usage values.
 * Useful for reducing noise in sampled RSS/VSZ readings. */

typedef struct {
    double alpha;       /* smoothing factor: 0 < alpha <= 1.0 */
    double value;       /* current smoothed value */
    int    initialized; /* 0 until first sample is applied */
    time_t last_update; /* timestamp of last update */
} decay_t;

/* Initialize decay smoother.
 * alpha=1.0 means no smoothing (raw value).
 * alpha=0.1 means heavy smoothing. */
void decay_init(decay_t *d, double alpha);

/* Feed a new raw sample; returns updated smoothed value. */
double decay_update(decay_t *d, double raw_value);

/* Reset smoother to uninitialized state. */
void decay_reset(decay_t *d);

/* Return current smoothed value (or 0.0 if not initialized). */
double decay_get(const decay_t *d);

/* Compute a one-shot EMA without storing state. */
double decay_ema(double prev, double raw, double alpha);

#endif /* DECAY_H */
