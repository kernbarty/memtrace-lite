#ifndef SUPPRESS_H
#define SUPPRESS_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* Suppress repeated alerts/events for a given key within a time window */

#define SUPPRESS_MAX_KEYS 64
#define SUPPRESS_KEY_LEN  32

typedef struct {
    char     key[SUPPRESS_KEY_LEN];
    uint32_t count;        /* times suppressed */
    time_t   first_seen;
    time_t   last_seen;
    bool     active;
} suppress_entry_t;

typedef struct {
    suppress_entry_t entries[SUPPRESS_MAX_KEYS];
    int      count;
    uint32_t window_sec;   /* suppression window in seconds */
} suppress_t;

void     suppress_init(suppress_t *s, uint32_t window_sec);
bool     suppress_check(suppress_t *s, const char *key); /* true = suppressed */
void     suppress_reset(suppress_t *s, const char *key);
void     suppress_reset_all(suppress_t *s);
uint32_t suppress_count(const suppress_t *s, const char *key);
void     suppress_expire(suppress_t *s);  /* remove expired entries */

#endif /* SUPPRESS_H */
