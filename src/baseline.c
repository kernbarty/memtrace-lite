#include "baseline.h"
#include <string.h>
#include <math.h>

#define MIN_READY_SAMPLES 8

void baseline_init(Baseline *b) {
    memset(b, 0, sizeof(*b));
}

void baseline_add(Baseline *b, double value, time_t now) {
    b->samples[b->head] = value;
    b->head = (b->head + 1) % BASELINE_MAX_SAMPLES;
    if (b->count < BASELINE_MAX_SAMPLES)
        b->count++;

    /* Recompute mean */
    double sum = 0.0;
    for (size_t i = 0; i < b->count; i++)
        sum += b->samples[i];
    b->mean = sum / (double)b->count;

    /* Recompute variance */
    double var = 0.0;
    for (size_t i = 0; i < b->count; i++) {
        double d = b->samples[i] - b->mean;
        var += d * d;
    }
    b->variance = (b->count > 1) ? var / (double)(b->count - 1) : 0.0;

    if (!b->ready && b->count >= MIN_READY_SAMPLES) {
        b->ready = 1;
        b->established_at = now;
    }
}

int baseline_is_ready(const Baseline *b) {
    return b->ready;
}

double baseline_deviation(const Baseline *b, double value) {
    double sd = baseline_stddev(b);
    if (sd < 1e-9) return 0.0;
    return (value - b->mean) / sd;
}

double baseline_mean(const Baseline *b) {
    return b->mean;
}

double baseline_stddev(const Baseline *b) {
    return sqrt(b->variance);
}

void baseline_reset(Baseline *b) {
    memset(b, 0, sizeof(*b));
}
