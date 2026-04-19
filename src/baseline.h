#ifndef BASELINE_H
#define BASELINE_H

#include <stddef.h>
#include <time.h>

#define BASELINE_MAX_SAMPLES 64

typedef struct {
    double samples[BASELINE_MAX_SAMPLES];
    size_t count;
    size_t head;
    double mean;
    double variance;
    time_t established_at;
    int ready;
} Baseline;

/* Initialize baseline state */
void baseline_init(Baseline *b);

/* Add a new sample and recompute statistics */
void baseline_add(Baseline *b, double value, time_t now);

/* Returns 1 if enough samples have been collected */
int baseline_is_ready(const Baseline *b);

/* Returns deviation of value from baseline mean in stddev units */
double baseline_deviation(const Baseline *b, double value);

/* Returns current mean */
double baseline_mean(const Baseline *b);

/* Returns current stddev */
double baseline_stddev(const Baseline *b);

/* Reset baseline */
void baseline_reset(Baseline *b);

#endif /* BASELINE_H */
