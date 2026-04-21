/*
 * watchdog.h - Periodic health check and recovery mechanism for memory monitoring
 *
 * Provides a watchdog timer that monitors the health of the memory tracer
 * itself, detecting stalls, hangs, or missed sampling intervals and
 * triggering recovery actions accordingly.
 */

#ifndef MEMTRACE_WATCHDOG_H
#define MEMTRACE_WATCHDOG_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum label length for watchdog instance identification */
#define WATCHDOG_LABEL_MAX 32

/* Watchdog states */
typedef enum {
    WATCHDOG_STATE_IDLE    = 0,  /* Not yet started */
    WATCHDOG_STATE_ACTIVE  = 1,  /* Running and healthy */
    WATCHDOG_STATE_EXPIRED = 2,  /* Timeout elapsed without a kick */
    WATCHDOG_STATE_TRIPPED = 3   /* Recovery action has been triggered */
} WatchdogState;

/* Recovery action callback: called when watchdog expires */
typedef void (*watchdog_action_fn)(void *userdata);

typedef struct {
    char              label[WATCHDOG_LABEL_MAX]; /* Human-readable name */
    uint64_t          timeout_ms;               /* Expiry window in milliseconds */
    watchdog_action_fn action;                  /* Called on expiry */
    void             *userdata;                 /* Passed to action callback */

    /* Internal state */
    WatchdogState     state;
    struct timespec   last_kick;                /* Time of last successful kick */
    uint32_t          trip_count;              /* Number of times watchdog tripped */
    uint64_t          total_kicks;             /* Lifetime kick counter */
} Watchdog;

/*
 * watchdog_init - Initialise a watchdog instance.
 *
 * @wd:         Pointer to Watchdog struct to initialise.
 * @label:      Short descriptive name (truncated to WATCHDOG_LABEL_MAX-1).
 * @timeout_ms: Milliseconds before the watchdog is considered expired.
 * @action:     Callback invoked when the watchdog expires (may be NULL).
 * @userdata:   Opaque pointer forwarded to @action.
 */
void watchdog_init(Watchdog *wd, const char *label, uint64_t timeout_ms,
                   watchdog_action_fn action, void *userdata);

/*
 * watchdog_kick - Reset the watchdog timer, indicating the system is alive.
 * Must be called within @timeout_ms to prevent expiry.
 */
void watchdog_kick(Watchdog *wd);

/*
 * watchdog_check - Evaluate whether the watchdog has expired.
 * If expired and an action is registered, the action is invoked once and
 * the state transitions to WATCHDOG_STATE_TRIPPED.
 *
 * Returns 1 if the watchdog has expired, 0 otherwise.
 */
int watchdog_check(Watchdog *wd);

/*
 * watchdog_reset - Reset state to ACTIVE and restart the timer.
 * Useful after a trip to re-arm the watchdog.
 */
void watchdog_reset(Watchdog *wd);

/* watchdog_state - Return the current WatchdogState. */
WatchdogState watchdog_state(const Watchdog *wd);

/* watchdog_trip_count - Return how many times the watchdog has tripped. */
uint32_t watchdog_trip_count(const Watchdog *wd);

/* watchdog_elapsed_ms - Milliseconds since the last kick (or init). */
uint64_t watchdog_elapsed_ms(const Watchdog *wd);

#ifdef __cplusplus
}
#endif

#endif /* MEMTRACE_WATCHDOG_H */
