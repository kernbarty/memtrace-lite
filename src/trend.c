#include "trend.h"
#include <math.h>
#include <string.h>

#define VOLATILE_VARIANCE_THRESHOLD 4000.0
#define GROWING_SLOPE_THRESHOLD      10.0
#define SHRINKING_SLOPE_THRESHOLD   -10.0

trend_result_t trend_analyze(const uint64_t *samples, size_t n) {
    trend_result_t result;
    memset(&result, 0, sizeof(result));
    result.sample_count = n;
    result.direction    = TREND_STABLE;

    if (n < 2) return result;

    /* Linear regression: y = slope * x + intercept */
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_xx = 0;
    for (size_t i = 0; i < n; i++) {
        double x = (double)i;
        double y = (double)samples[i];
        sum_x  += x;
        sum_y  += y;
        sum_xy += x * y;
        sum_xx += x * x;
    }
    double dn = (double)n;
    double denom = dn * sum_xx - sum_x * sum_x;
    double slope = 0.0;
    if (denom != 0.0)
        slope = (dn * sum_xy - sum_x * sum_y) / denom;

    /* Variance of samples */
    double mean = sum_y / dn;
    double var  = 0.0;
    for (size_t i = 0; i < n; i++) {
        double diff = (double)samples[i] - mean;
        var += diff * diff;
    }
    var /= dn;

    result.slope_kb_per_sample = slope;
    result.variance            = var;

    if (var > VOLATILE_VARIANCE_THRESHOLD) {
        result.direction = TREND_VOLATILE;
    } else if (slope > GROWING_SLOPE_THRESHOLD) {
        result.direction = TREND_GROWING;
    } else if (slope < SHRINKING_SLOPE_THRESHOLD) {
        result.direction = TREND_SHRINKING;
    } else {
        result.direction = TREND_STABLE;
    }

    return result;
}

const char *trend_direction_str(trend_direction_t dir) {
    switch (dir) {
        case TREND_STABLE:    return "stable";
        case TREND_GROWING:   return "growing";
        case TREND_SHRINKING: return "shrinking";
        case TREND_VOLATILE:  return "volatile";
        default:              return "unknown";
    }
}
