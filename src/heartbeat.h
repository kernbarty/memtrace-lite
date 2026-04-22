#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <time.h>
#include <stdbool.h>

/* Heartbeat module: periodic liveness signal for monitored processes.
 * Tracks last-seen timestamps and detects missed beats.
 */

#define HEARTBEAT_MAX_MISSED 5

typedef enum {
    HEARTBEAT_ALIVE  = 0,
    HEARTBEAT_STALE  = 1,
    HEARTBEAT_DEAD   = 2
} HeartbeatStatus;

typedef struct {
    pid_t        pid;
    time_t       last_beat;      /* epoch seconds of last recorded beat */
    unsigned int interval_sec;   /* expected beat interval in seconds   */
    unsigned int missed;         /* consecutive missed beats             */
    HeartbeatStatus status;
} Heartbeat;

/* Initialise a heartbeat record for the given pid and expected interval. */
void heartbeat_init(Heartbeat *hb, pid_t pid, unsigned int interval_sec);

/* Record that the process is alive right now. Resets missed counter. */
void heartbeat_pulse(Heartbeat *hb);

/* Evaluate the heartbeat against the current time.
 * Increments missed counter if the deadline has passed.
 * Returns the updated status. */
HeartbeatStatus heartbeat_check(Heartbeat *hb);

/* Return true if the heartbeat is considered dead
 * (missed >= HEARTBEAT_MAX_MISSED). */
bool heartbeat_is_dead(const Heartbeat *hb);

/* Reset the heartbeat to ALIVE state without updating last_beat. */
void heartbeat_reset(Heartbeat *hb);

/* Human-readable status string. */
const char *heartbeat_status_str(HeartbeatStatus s);

#endif /* HEARTBEAT_H */
