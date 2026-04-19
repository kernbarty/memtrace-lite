#include "jitter.h"
#include <stddef.h>

/* Simple LCG RNG for minimal overhead */
static uint32_t lcg_next(uint32_t *state) {
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static uint32_t rand_range(uint32_t *state, uint32_t lo, uint32_t hi) {
    if (lo >= hi) return lo;
    return lo + (lcg_next(state) % (hi - lo));
}

void jitter_init(jitter_t *j, jitter_mode_t mode, uint32_t base_ms,
                 uint32_t cap_ms, uint32_t seed) {
    if (!j) return;
    j->mode    = mode;
    j->base_ms = base_ms;
    j->cap_ms  = cap_ms;
    j->seed    = seed ? seed : 0xdeadbeef;
}

uint32_t jitter_apply(jitter_t *j, uint32_t delay_ms) {
    if (!j) return delay_ms;
    switch (j->mode) {
        case JITTER_NONE:
            return delay_ms;
        case JITTER_FULL:
            return delay_ms == 0 ? 0 : (lcg_next(&j->seed) % (delay_ms + 1));
        case JITTER_EQUAL: {
            uint32_t half = delay_ms / 2;
            return rand_range(&j->seed, half, delay_ms ? delay_ms : 1);
        }
        case JITTER_DECORR: {
            /* decorrelated: sleep = min(cap, rand(base, prev*3)) */
            uint32_t lo = j->base_ms;
            uint32_t hi = delay_ms * 3;
            if (hi < lo) hi = lo + 1;
            uint32_t v = rand_range(&j->seed, lo, hi);
            return v < j->cap_ms ? v : j->cap_ms;
        }
        default:
            return delay_ms;
    }
}

void jitter_reset(jitter_t *j) {
    if (!j) return;
    j->seed = 0xdeadbeef;
}
