#ifndef TRACKER_H
#define TRACKER_H

#include "snapshot.h"
#include <stddef.h>
#include <time.h>

#define TRACKER_MAX_SNAPSHOTS 1024

typedef struct {
    pid_t        pid;
    unsigned int interval_ms;
    size_t       capacity;
    size_t       count;
    MemSnapshot *snapshots;
    struct timespec start_time;
} MemTracker;

/* Initialize a tracker for the given pid.
 * interval_ms: sampling interval in milliseconds.
 * Returns 0 on success, -1 on failure. */
int tracker_init(MemTracker *t, pid_t pid, unsigned int interval_ms);

/* Collect one snapshot and append it to the tracker's history.
 * Returns 0 on success, -1 on failure or when capacity is reached. */
int tracker_collect(MemTracker *t);

/* Return peak RSS (kB) seen across all collected snapshots. */
long tracker_peak_rss(const MemTracker *t);

/* Free all resources held by the tracker. */
void tracker_free(MemTracker *t);

#endif /* TRACKER_H */
