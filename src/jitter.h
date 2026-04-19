#ifndef MEMTRACE_JITTER_H
#define MEMTRACE_JITTER_H

#include <stdint.h>

/* Jitter adds randomized variance to delay values to avoid thundering herd */

typedef enum {
    JITTER_NONE = 0,
    JITTER_FULL,      /* [0, delay] */
    JITTER_EQUAL,     /* [delay/2, delay] */
    JITTER_DECORR     /* decorrelated: min(cap, rand(base, prev*3)) */
} jitter_mode_t;

typedef struct {
    jitter_mode_t mode;
    uint32_t base_ms;
    uint32_t cap_ms;
    uint32_t seed;
} jitter_t;

void     jitter_init(jitter_t *j, jitter_mode_t mode, uint32_t base_ms,
                     uint32_t cap_ms, uint32_t seed);
uint32_t jitter_apply(jitter_t *j, uint32_t delay_ms);
void     jitter_reset(jitter_t *j);

#endif /* MEMTRACE_JITTER_H */
