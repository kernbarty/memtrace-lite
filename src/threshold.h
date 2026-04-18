#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    THRESHOLD_RSS,
    THRESHOLD_VSZ,
    THRESHOLD_DELTA
} threshold_metric_t;

typedef enum {
    THRESHOLD_WARN,
    THRESHOLD_CRITICAL
} threshold_level_t;

typedef struct {
    threshold_metric_t metric;
    threshold_level_t  level;
    uint64_t           value_kb;
    char               label[64];
} threshold_t;

typedef struct {
    threshold_t *entries;
    int          count;
    int          capacity;
} threshold_set_t;

int  threshold_set_init(threshold_set_t *ts, int capacity);
void threshold_set_free(threshold_set_t *ts);
int  threshold_add(threshold_set_t *ts, threshold_metric_t metric,
                   threshold_level_t level, uint64_t value_kb,
                   const char *label);
bool threshold_check(const threshold_set_t *ts, threshold_metric_t metric,
                     uint64_t value_kb, threshold_level_t *out_level,
                     const char **out_label);

#endif /* THRESHOLD_H */
