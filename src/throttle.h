#ifndef THROTTLE_H
#define THROTTLE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* Throttle: rate-limit sampling or alerting actions */

typedef struct {
    uint64_t interval_ms;   /* minimum ms between allowed events */
    uint64_t last_ms;       /* timestamp of last allowed event */
    uint64_t suppressed;    /* count of suppressed events */
    bool     enabled;       /* if false, always allow */
} Throttle;

/* Initialize throttle with interval in milliseconds */
void throttle_init(Throttle *t, uint64_t interval_ms);

/* Enable or disable throttling */
void throttle_set_enabled(Throttle *t, bool enabled);

/* Returns true if event is allowed (not throttled); updates state */
bool throttle_allow(Throttle *t);

/* Reset suppression counter and last timestamp */
void throttle_reset(Throttle *t);

/* Return number of suppressed events since last reset */
uint64_t throttle_suppressed(const Throttle *t);

/* Return current monotonic time in ms */
uint64_t throttle_now_ms(void);

#endif /* THROTTLE_H */
