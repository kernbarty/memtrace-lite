#include "epoch.h"
#include <string.h>

static void timespec_now(struct timespec *ts) {
    clock_gettime(CLOCK_MONOTONIC, ts);
}

static int timespec_is_zero(const struct timespec *ts) {
    return ts->tv_sec == 0 && ts->tv_nsec == 0;
}

static uint64_t timespec_diff_ms(const struct timespec *a,
                                  const struct timespec *b) {
    int64_t sec  = (int64_t)b->tv_sec  - (int64_t)a->tv_sec;
    int64_t nsec = (int64_t)b->tv_nsec - (int64_t)a->tv_nsec;
    if (nsec < 0) {
        sec  -= 1;
        nsec += 1000000000LL;
    }
    if (sec < 0) return 0;
    return (uint64_t)(sec * 1000 + nsec / 1000000);
}

void epoch_open(Epoch *e, uint64_t seq) {
    memset(e, 0, sizeof(*e));
    e->seq = seq;
    timespec_now(&e->start);
}

void epoch_close(Epoch *e) {
    if (epoch_is_open(e)) {
        timespec_now(&e->end);
    }
}

uint64_t epoch_elapsed_ms(const Epoch *e) {
    struct timespec ref;
    if (timespec_is_zero(&e->end)) {
        timespec_now(&ref);
    } else {
        ref = e->end;
    }
    return timespec_diff_ms(&e->start, &ref);
}

int epoch_is_open(const Epoch *e) {
    return timespec_is_zero(&e->end);
}

void epoch_record(Epoch *e, uint64_t bytes) {
    e->sample_count++;
    e->bytes_total += bytes;
}

void epoch_reset(Epoch *e) {
    memset(e, 0, sizeof(*e));
}
