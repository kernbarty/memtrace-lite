#ifndef BACKOFF_H
#define BACKOFF_H

#include <stdint.h>
#include <stdbool.h>

/* Exponential backoff with jitter for retry/sampling control */

#define BACKOFF_MIN_MS   10
#define BACKOFF_MAX_MS   5000

typedef struct {
    uint32_t min_ms;      /* minimum delay in ms */
    uint32_t max_ms;      /* maximum delay cap in ms */
    uint32_t current_ms;  /* current delay */
    uint32_t attempts;    /* number of consecutive backoffs */
    bool     jitter;      /* apply random jitter */
} backoff_t;

/* Initialize backoff with given min/max and optional jitter */
void backoff_init(backoff_t *b, uint32_t min_ms, uint32_t max_ms, bool jitter);

/* Reset backoff to initial state */
void backoff_reset(backoff_t *b);

/* Advance backoff: doubles current delay (capped at max), returns new delay ms */
uint32_t backoff_next(backoff_t *b);

/* Returns current delay without advancing */
uint32_t backoff_current(const backoff_t *b);

/* Returns true if backoff has reached the maximum delay */
bool backoff_saturated(const backoff_t *b);

#endif /* BACKOFF_H */
