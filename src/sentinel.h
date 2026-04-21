/*
 * sentinel.h - Watchpoint-style sentinel for memory region boundary violations
 *
 * Monitors a named memory metric against warn/critical thresholds.
 * Fires a callback on state transitions and tracks trip counts.
 */

#ifndef SENTINEL_H
#define SENTINEL_H

#include <stddef.h>
#include <stdio.h>

#define SENTINEL_NAME_MAX 64

typedef enum {
    SENTINEL_OK       = 0,
    SENTINEL_WARN     = 1,
    SENTINEL_CRITICAL = 2
} SentinelState;

typedef struct {
    char          name[SENTINEL_NAME_MAX];
    SentinelState state;
    size_t        value_kb;
    unsigned int  trip_count;
} SentinelEvent;

typedef void (*sentinel_cb)(const SentinelEvent *ev, void *user_data);

typedef struct {
    char          name[SENTINEL_NAME_MAX];
    size_t        warn_kb;        /* warn threshold in KB (0 = disabled) */
    size_t        critical_kb;    /* critical threshold in KB (0 = disabled) */
    SentinelState state;          /* current state */
    size_t        last_value_kb;  /* last observed value */
    unsigned int  trip_count;     /* total number of threshold crossings */
    sentinel_cb   callback;       /* optional event callback */
    void         *user_data;      /* opaque pointer passed to callback */
} Sentinel;

/* Initialise sentinel with name and thresholds */
void sentinel_init(Sentinel *s, const char *name, size_t warn_kb, size_t critical_kb);

/* Register a callback fired on each threshold trip */
void sentinel_set_callback(Sentinel *s, sentinel_cb cb, void *user_data);

/* Feed a new sample; returns resulting state */
SentinelState sentinel_check(Sentinel *s, size_t current_kb);

/* Reset state and trip counter (thresholds preserved) */
void sentinel_reset(Sentinel *s);

/* Human-readable state label */
const char *sentinel_state_str(SentinelState state);

/* Print sentinel summary to file stream */
void sentinel_dump(const Sentinel *s, FILE *out);

#endif /* SENTINEL_H */
