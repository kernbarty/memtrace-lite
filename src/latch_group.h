#ifndef MEMTRACE_LATCH_GROUP_H
#define MEMTRACE_LATCH_GROUP_H

#include "latch.h"
#include <stddef.h>

#define LATCH_GROUP_MAX 8

/*
 * latch_group — manage a small array of named latches together.
 *
 * Allows bulk operations (reset-all, count-triggered) and lookup
 * by label, which is handy when several independent conditions
 * (rss_high, vms_spike, swap_nonzero, …) must each latch once.
 */

typedef struct {
    latch_t      latches[LATCH_GROUP_MAX];
    unsigned int count;
} latch_group_t;

void         latch_group_init(latch_group_t *g);

/* Add a named, armed latch. Returns index or -1 on overflow. */
int          latch_group_add(latch_group_t *g, const char *label);

/* Find a latch by label; returns NULL if not found. */
latch_t     *latch_group_find(latch_group_t *g, const char *label);

/* Trigger a latch by label. Returns true on first trigger. */
bool         latch_group_trigger(latch_group_t *g, const char *label,
                                 uint64_t value);

/* Number of latches currently in triggered state. */
unsigned int latch_group_triggered_count(const latch_group_t *g);

/* Reset all latches back to armed state. */
void         latch_group_reset_all(latch_group_t *g);

#endif /* MEMTRACE_LATCH_GROUP_H */
