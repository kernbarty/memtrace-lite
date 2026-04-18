#ifndef MEMTRACE_STATS_H
#define MEMTRACE_STATS_H

#include <stddef.h>
#include <stdint.h>

/* Aggregate statistics computed from a series of memory samples */
typedef struct {
    size_t sample_count;
    long   min_rss_kb;
    long   max_rss_kb;
    double avg_rss_kb;
    long   min_vms_kb;
    long   max_vms_kb;
    double avg_vms_kb;
    long   peak_rss_kb;   /* high-water mark across all samples */
    long   delta_rss_kb;  /* last - first rss */
} MemStats;

/*
 * Reset stats to initial state.
 */
void stats_init(MemStats *s);

/*
 * Feed one (rss_kb, vms_kb) observation into the running stats.
 */
void stats_update(MemStats *s, long rss_kb, long vms_kb);

/*
 * Finalise averages.  Call once after all stats_update() calls.
 */
void stats_finalise(MemStats *s);

#endif /* MEMTRACE_STATS_H */
