/*
 * window.c - Sliding window aggregation over memory samples
 *
 * Maintains a fixed-size time window of memory snapshots and provides
 * aggregated statistics (min, max, mean, sum) over the window contents.
 */

#include "window.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* Initialize a sliding window with a fixed capacity */
void window_init(Window *w, size_t capacity) {
    assert(w != NULL);
    assert(capacity > 0 && capacity <= WINDOW_MAX_SIZE);

    w->capacity = capacity;
    w->count    = 0;
    w->head     = 0;
    memset(w->samples, 0, sizeof(w->samples));
}

/* Reset window to empty state without changing capacity */
void window_reset(Window *w) {
    assert(w != NULL);
    w->count = 0;
    w->head  = 0;
    memset(w->samples, 0, sizeof(w->samples));
}

/* Push a new sample into the window, evicting the oldest if full */
void window_push(Window *w, const WindowSample *sample) {
    assert(w != NULL);
    assert(sample != NULL);

    w->samples[w->head] = *sample;
    w->head = (w->head + 1) % w->capacity;

    if (w->count < w->capacity)
        w->count++;
}

/* Push a raw value with the given timestamp */
void window_push_value(Window *w, uint64_t timestamp_ms, uint64_t value_kb) {
    WindowSample s = { .timestamp_ms = timestamp_ms, .value_kb = value_kb };
    window_push(w, &s);
}

/* Returns true if the window has no samples */
bool window_empty(const Window *w) {
    assert(w != NULL);
    return w->count == 0;
}

/* Returns true if the window has reached its capacity */
bool window_full(const Window *w) {
    assert(w != NULL);
    return w->count == w->capacity;
}

/* Compute aggregate statistics over all samples currently in the window */
bool window_stats(const Window *w, WindowStats *out) {
    assert(w != NULL);
    assert(out != NULL);

    if (w->count == 0)
        return false;

    uint64_t sum  = 0;
    uint64_t vmin = UINT64_MAX;
    uint64_t vmax = 0;

    /* Iterate from oldest to newest sample */
    for (size_t i = 0; i < w->count; i++) {
        size_t idx = (w->head + w->capacity - w->count + i) % w->capacity;
        uint64_t v = w->samples[idx].value_kb;

        sum += v;
        if (v < vmin) vmin = v;
        if (v > vmax) vmax = v;
    }

    out->count   = w->count;
    out->sum_kb  = sum;
    out->min_kb  = vmin;
    out->max_kb  = vmax;
    out->mean_kb = sum / w->count;

    /* Oldest sample timestamp */
    size_t oldest_idx = (w->head + w->capacity - w->count) % w->capacity;
    out->oldest_ts_ms = w->samples[oldest_idx].timestamp_ms;

    /* Newest sample timestamp (head - 1 wrapping) */
    size_t newest_idx = (w->head + w->capacity - 1) % w->capacity;
    out->newest_ts_ms = w->samples[newest_idx].timestamp_ms;

    return true;
}

/* Retrieve the Nth sample (0 = oldest, count-1 = newest) */
bool window_get(const Window *w, size_t n, WindowSample *out) {
    assert(w != NULL);
    assert(out != NULL);

    if (n >= w->count)
        return false;

    size_t idx = (w->head + w->capacity - w->count + n) % w->capacity;
    *out = w->samples[idx];
    return true;
}
