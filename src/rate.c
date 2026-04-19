#include "rate.h"
#include <stdio.h>
#include <string.h>

static double timespec_diff_sec(const struct timespec *a, const struct timespec *b) {
    return (double)(a->tv_sec - b->tv_sec)
         + (double)(a->tv_nsec - b->tv_nsec) / 1e9;
}

void rate_init(MemRate *r) {
    memset(r, 0, sizeof(*r));
}

void rate_update(MemRate *r, long kb) {
    if (!r->valid) {
        r->prev_kb = kb;
        clock_gettime(CLOCK_MONOTONIC, &r->prev_time);
        r->curr_kb = kb;
        r->curr_time = r->prev_time;
        r->valid = 0;
        /* mark as having one sample */
        r->rate_kb_per_sec = 0.0;
        /* use a sentinel: store kb in curr and flag with -1 */
        r->prev_kb = kb;
        r->valid = -1; /* one sample */
        return;
    }

    if (r->valid == -1) {
        /* second sample */
        r->curr_kb = kb;
        clock_gettime(CLOCK_MONOTONIC, &r->curr_time);
        double dt = timespec_diff_sec(&r->curr_time, &r->prev_time);
        if (dt > 0.0)
            r->rate_kb_per_sec = (double)(r->curr_kb - r->prev_kb) / dt;
        else
            r->rate_kb_per_sec = 0.0;
        r->valid = 1;
        return;
    }

    /* subsequent samples: slide window */
    r->prev_kb = r->curr_kb;
    r->prev_time = r->curr_time;
    r->curr_kb = kb;
    clock_gettime(CLOCK_MONOTONIC, &r->curr_time);
    double dt = timespec_diff_sec(&r->curr_time, &r->prev_time);
    if (dt > 0.0)
        r->rate_kb_per_sec = (double)(r->curr_kb - r->prev_kb) / dt;
    else
        r->rate_kb_per_sec = 0.0;
}

double rate_get(const MemRate *r) {
    if (r->valid != 1) return 0.0;
    return r->rate_kb_per_sec;
}

int rate_exceeds(const MemRate *r, double threshold_kb_per_sec) {
    return rate_get(r) > threshold_kb_per_sec;
}

void rate_format(const MemRate *r, char *buf, int bufsz) {
    double v = rate_get(r);
    snprintf(buf, bufsz, "%+.1f KB/s", v);
}
