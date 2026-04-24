#ifndef MEMTRACE_LATCH_H
#define MEMTRACE_LATCH_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/*
 * latch — one-shot edge-triggered condition latch
 *
 * A latch arms on first trigger and stays set until explicitly reset.
 * Useful for capturing the first occurrence of a memory condition
 * (e.g. first time RSS exceeds a threshold) without re-firing on
 * every subsequent sample.
 */

typedef enum {
    LATCH_DISARMED = 0,
    LATCH_ARMED,
    LATCH_TRIGGERED
} latch_state_t;

typedef struct {
    latch_state_t  state;
    uint64_t       trigger_value;   /* value that caused the trigger  */
    time_t         trigger_time;    /* wall-clock time of trigger      */
    unsigned int   trigger_count;   /* how many times trigger() called */
    const char    *label;           /* optional human-readable tag     */
} latch_t;

/* Initialise and arm a latch. label may be NULL. */
void latch_init(latch_t *l, const char *label);

/* Arm a previously disarmed or reset latch. */
void latch_arm(latch_t *l);

/*
 * Attempt to trigger the latch with a sample value.
 * Returns true on the FIRST trigger; false if already triggered or disarmed.
 */
bool latch_trigger(latch_t *l, uint64_t value);

/* Returns true if the latch has been triggered. */
bool latch_is_triggered(const latch_t *l);

/* Reset to armed state, clearing trigger info. */
void latch_reset(latch_t *l);

/* Disarm completely — trigger() becomes a no-op until latch_arm(). */
void latch_disarm(latch_t *l);

#endif /* MEMTRACE_LATCH_H */
