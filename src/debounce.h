#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <stdint.h>
#include <stdbool.h>

/* Debounce: suppress repeated triggers within a quiet window */

typedef struct {
    uint64_t window_ms;      /* minimum ms between accepted triggers */
    uint64_t last_trigger_ms;
    bool     armed;          /* true after first trigger seen */
} Debounce;

/*
 * Initialize debounce with a quiet window in milliseconds.
 */
void debounce_init(Debounce *db, uint64_t window_ms);

/*
 * Feed a trigger at time now_ms.
 * Returns true if the trigger is accepted (outside the quiet window),
 * false if it is suppressed.
 */
bool debounce_trigger(Debounce *db, uint64_t now_ms);

/*
 * Reset debounce state so next trigger is always accepted.
 */
void debounce_reset(Debounce *db);

/*
 * Returns ms remaining in the quiet window, or 0 if ready.
 */
uint64_t debounce_remaining(const Debounce *db, uint64_t now_ms);

#endif /* DEBOUNCE_H */
