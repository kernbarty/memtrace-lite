#ifndef SUPPRESS_H
#define SUPPRESS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SUPPRESS_MODE_TIMED,   /* suppress for a fixed duration after first event */
    SUPPRESS_MODE_COUNT    /* suppress after max_count events in window */
} suppress_mode_t;

typedef struct {
    suppress_mode_t mode;
    uint32_t        window_ms;        /* window or suppression duration in ms */
    uint32_t        max_count;        /* threshold for COUNT mode */
    uint32_t        count;            /* events recorded in current window */
    uint32_t        window_start_ms;  /* start of current window */
    uint32_t        suppress_until_ms;/* end of timed suppression */
    bool            active;           /* currently suppressing */
} suppress_t;

/* Initialize suppressor */
void suppress_init(suppress_t *s, suppress_mode_t mode, uint32_t window_ms, uint32_t max_count);

/* Reset suppressor state */
void suppress_reset(suppress_t *s);

/* Check if currently suppressed without recording an event */
bool suppress_check(suppress_t *s, uint32_t now_ms);

/* Record an event and return true if it should be suppressed */
bool suppress_record(suppress_t *s, uint32_t now_ms);

/* Get number of events recorded in current window */
uint32_t suppress_get_count(const suppress_t *s);

/* Check if suppressor is active */
bool suppress_is_active(const suppress_t *s);

#endif /* SUPPRESS_H */
