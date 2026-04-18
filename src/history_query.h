#pragma once
#include "history.h"
#include "snapshot.h"
#include <stdbool.h>
#include <stddef.h>

/* Query results over a History ring buffer */

typedef struct {
    long rss_delta_kb;
    long vms_delta_kb;
    double rss_avg_kb;
    double vms_avg_kb;
    long rss_peak_kb;
    long vms_peak_kb;
    size_t sample_count;
} HistoryStats;

/* Compute aggregate stats over all entries in history */
bool history_query_stats(const History *h, HistoryStats *out);

/* Find snapshot with maximum RSS */
bool history_query_peak_rss(const History *h, MemSnapshot *out);

/* Compute delta between oldest and latest snapshot */
bool history_query_delta(const History *h, long *rss_delta_kb, long *vms_delta_kb);

/* Find index of first snapshot exceeding rss_threshold_kb */
bool history_query_find_rss_exceed(const History *h, long rss_threshold_kb, size_t *index_out);
