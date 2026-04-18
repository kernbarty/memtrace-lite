#ifndef FILTER_H
#define FILTER_H

#include <stddef.h>

/* Filter criteria for memory snapshots */
typedef enum {
    FILTER_NONE        = 0,
    FILTER_MIN_RSS     = 1 << 0,
    FILTER_MAX_RSS     = 1 << 1,
    FILTER_MIN_VSZ     = 1 << 2,
    FILTER_MAX_VSZ     = 1 << 3,
    FILTER_NAME_SUBSTR = 1 << 4
} FilterFlags;

typedef struct {
    FilterFlags flags;
    long        min_rss_kb;
    long        max_rss_kb;
    long        min_vsz_kb;
    long        max_vsz_kb;
    char        name_substr[64];
} MemFilter;

#include "snapshot.h"

/* Initialize filter to match everything */
void filter_init(MemFilter *f);

/* Set RSS range filter (kb, -1 to ignore bound) */
void filter_set_rss(MemFilter *f, long min_kb, long max_kb);

/* Set VSZ range filter (kb, -1 to ignore bound) */
void filter_set_vsz(MemFilter *f, long min_kb, long max_kb);

/* Set substring match on snapshot label */
void filter_set_name(MemFilter *f, const char *substr);

/* Returns 1 if snapshot passes filter, 0 otherwise */
int filter_match(const MemFilter *f, const MemSnapshot *snap);

#endif /* FILTER_H */
