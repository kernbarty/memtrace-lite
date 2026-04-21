/*
 * window.h - Sliding time window aggregator for memory metrics
 *
 * Maintains a fixed-size rolling window of samples and provides
 * aggregate statistics (min, max, avg, sum) over that window.
 */

#ifndef MEMTRACE_WINDOW_H
#define MEMTRACE_WINDOW_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#define WINDOW_MAX_SLOTS 128

typedef struct {
    uint64_t value;
    time_t   timestamp;
} window_slot_t;

typedef struct {
    window_slot_t slots[WINDOW_MAX_SLOTS];
    size_t        capacity;   /* number of slots in use (<= WINDOW_MAX_SLOTS) */
    size_t        head;       /* index of next write position */
    size_t        count;      /* number of valid entries */
    unsigned int  duration_s; /* time span covered by the window in seconds */
} window_t;

typedef struct {
    uint64_t min;
    uint64_t max;
    uint64_t sum;
    double   avg;
    size_t   count;
} window_stats_t;

/* Initialise a window with the given slot capacity and time duration. */
int  window_init(window_t *w, size_t capacity, unsigned int duration_s);

/* Push a new sample into the window (evicts expired/oldest entries). */
void window_push(window_t *w, uint64_t value, time_t now);

/* Compute aggregate statistics over all valid (non-expired) slots. */
window_stats_t window_aggregate(const window_t *w, time_t now);

/* Return the number of valid (non-expired) entries in the window. */
size_t window_count(const window_t *w, time_t now);

/* Reset the window, discarding all samples. */
void window_reset(window_t *w);

#endif /* MEMTRACE_WINDOW_H */
