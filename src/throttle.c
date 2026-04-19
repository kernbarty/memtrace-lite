#include "throttle.h"
#include <string.h>

uint64_t throttle_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000);
}

void throttle_init(Throttle *t, uint64_t interval_ms) {
    if (!t) return;
    memset(t, 0, sizeof(*t));
    t->interval_ms = interval_ms;
    t->enabled     = true;
    t->last_ms     = 0;
    t->suppressed  = 0;
}

void throttle_set_enabled(Throttle *t, bool enabled) {
    if (!t) return;
    t->enabled = enabled;
}

bool throttle_allow(Throttle *t) {
    if (!t) return true;
    if (!t->enabled) return true;

    uint64_t now = throttle_now_ms();
    if (t->last_ms == 0 || (now - t->last_ms) >= t->interval_ms) {
        t->last_ms = now;
        return true;
    }
    t->suppressed++;
    return false;
}

void throttle_reset(Throttle *t) {
    if (!t) return;
    t->last_ms    = 0;
    t->suppressed = 0;
}

uint64_t throttle_suppressed(const Throttle *t) {
    if (!t) return 0;
    return t->suppressed;
}
