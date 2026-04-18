#include "tracker.h"
#include "procmem.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int tracker_init(MemTracker *t, pid_t pid, unsigned int interval_ms)
{
    if (!t) return -1;

    t->snapshots = calloc(TRACKER_MAX_SNAPSHOTS, sizeof(MemSnapshot));
    if (!t->snapshots) return -1;

    t->pid         = pid;
    t->interval_ms = interval_ms;
    t->capacity    = TRACKER_MAX_SNAPSHOTS;
    t->count       = 0;
    clock_gettime(CLOCK_MONOTONIC, &t->start_time);
    return 0;
}

int tracker_collect(MemTracker *t)
{
    if (!t || t->count >= t->capacity) return -1;

    MemSnapshot *snap = &t->snapshots[t->count];
    memset(snap, 0, sizeof(*snap));

    if (procmem_read(t->pid, snap) != 0)
        return -1;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    snap->timestamp_ms =
        (long long)(now.tv_sec  - t->start_time.tv_sec)  * 1000 +
        (now.tv_nsec - t->start_time.tv_nsec) / 1000000;

    t->count++;
    return 0;
}

long tracker_peak_rss(const MemTracker *t)
{
    if (!t || t->count == 0) return 0;

    long peak = 0;
    for (size_t i = 0; i < t->count; i++) {
        if (t->snapshots[i].rss_kb > peak)
            peak = t->snapshots[i].rss_kb;
    }
    return peak;
}

void tracker_free(MemTracker *t)
{
    if (!t) return;
    free(t->snapshots);
    t->snapshots = NULL;
    t->count     = 0;
    t->capacity  = 0;
}
