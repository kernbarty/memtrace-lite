#ifndef TREND_H
#define TREND_H

#include <stddef.h>
#include <stdint.h>

/* Direction of memory usage trend */
typedef enum {
    TREND_STABLE   = 0,
    TREND_GROWING  = 1,
    TREND_SHRINKING = 2,
    TREND_VOLATILE = 3
} trend_direction_t;

typedef struct {
    trend_direction_t direction;
    double            slope_kb_per_sample; /* linear regression slope */
    double            variance;            /* variance of samples */
    size_t            sample_count;
} trend_result_t;

/*
 * Analyze an array of rss_kb samples (oldest first).
 * Returns TREND_STABLE if sample_count < 2.
 */
trend_result_t trend_analyze(const uint64_t *samples, size_t sample_count);

/* Return a human-readable string for a trend direction. */
const char *trend_direction_str(trend_direction_t dir);

#endif /* TREND_H */
