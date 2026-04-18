#include "threshold.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int threshold_set_init(threshold_set_t *ts, int capacity) {
    if (!ts || capacity <= 0) return -1;
    ts->entries = calloc(capacity, sizeof(threshold_t));
    if (!ts->entries) return -1;
    ts->count    = 0;
    ts->capacity = capacity;
    return 0;
}

void threshold_set_free(threshold_set_t *ts) {
    if (!ts) return;
    free(ts->entries);
    ts->entries  = NULL;
    ts->count    = 0;
    ts->capacity = 0;
}

int threshold_add(threshold_set_t *ts, threshold_metric_t metric,
                  threshold_level_t level, uint64_t value_kb,
                  const char *label) {
    if (!ts || ts->count >= ts->capacity) return -1;
    threshold_t *e = &ts->entries[ts->count++];
    e->metric   = metric;
    e->level    = level;
    e->value_kb = value_kb;
    snprintf(e->label, sizeof(e->label), "%s", label ? label : "");
    return 0;
}

bool threshold_check(const threshold_set_t *ts, threshold_metric_t metric,
                     uint64_t value_kb, threshold_level_t *out_level,
                     const char **out_label) {
    if (!ts) return false;

    bool triggered        = false;
    threshold_level_t top = THRESHOLD_WARN;
    const char *top_label = NULL;

    for (int i = 0; i < ts->count; i++) {
        const threshold_t *e = &ts->entries[i];
        if (e->metric == metric && value_kb >= e->value_kb) {
            if (!triggered || e->level > top) {
                top       = e->level;
                top_label = e->label;
            }
            triggered = true;
        }
    }

    if (triggered) {
        if (out_level) *out_level = top;
        if (out_label) *out_label = top_label;
    }
    return triggered;
}
