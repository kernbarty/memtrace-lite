#include "backoff.h"
#include <stddef.h>

void backoff_init(backoff_t *b, const backoff_config_t *cfg) {
    if (!b || !cfg) return;
    b->cfg = *cfg;
    b->current_ms = cfg->initial_ms;
    b->attempt = 0;
    b->active = 0;
}

void backoff_reset(backoff_t *b) {
    if (!b) return;
    b->current_ms = b->cfg.initial_ms;
    b->attempt = 0;
    b->active = 0;
}

uint32_t backoff_next(backoff_t *b) {
    if (!b) return 0;
    b->active = 1;
    uint32_t val = b->current_ms;
    b->attempt++;
    uint32_t next = b->current_ms * b->cfg.multiplier;
    if (next > b->cfg.max_ms || next < b->current_ms)
        next = b->cfg.max_ms;
    b->current_ms = next;
    return val;
}

int backoff_exceeded(const backoff_t *b) {
    if (!b) return 0;
    return b->cfg.max_attempts > 0 && b->attempt >= b->cfg.max_attempts;
}

uint32_t backoff_current(const backoff_t *b) {
    if (!b) return 0;
    return b->current_ms;
}

uint32_t backoff_attempt(const backoff_t *b) {
    if (!b) return 0;
    return b->attempt;
}
