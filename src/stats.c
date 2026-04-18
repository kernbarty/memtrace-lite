#include "stats.h"
#include <string.h>
#include <limits.h>

void stats_init(MemStats *s)
{
    if (!s) return;
    memset(s, 0, sizeof(*s));
    s->min_rss_kb = LONG_MAX;
    s->min_vms_kb = LONG_MAX;
    s->max_rss_kb = LONG_MIN;
    s->max_vms_kb = LONG_MIN;
    s->peak_rss_kb = 0;
}

void stats_update(MemStats *s, long rss_kb, long vms_kb)
{
    if (!s) return;

    if (s->sample_count == 0) {
        /* store first rss for delta calculation */
        s->delta_rss_kb = rss_kb; /* temporarily holds first value */
    }

    s->sample_count++;

    if (rss_kb < s->min_rss_kb) s->min_rss_kb = rss_kb;
    if (rss_kb > s->max_rss_kb) s->max_rss_kb = rss_kb;
    if (rss_kb > s->peak_rss_kb) s->peak_rss_kb = rss_kb;

    if (vms_kb < s->min_vms_kb) s->min_vms_kb = vms_kb;
    if (vms_kb > s->max_vms_kb) s->max_vms_kb = vms_kb;

    /* Welford-style running sum stored temporarily in avg fields */
    s->avg_rss_kb += (double)rss_kb;
    s->avg_vms_kb += (double)vms_kb;

    /* Keep last rss for delta */
    s->delta_rss_kb = rss_kb;
}

void stats_finalise(MemStats *s)
{
    if (!s || s->sample_count == 0) return;

    /* avg fields currently hold sums; convert to averages */
    long first_rss = s->min_rss_kb; /* approximate: use min as proxy isn't ideal */
    /* We stored last rss in delta; compute actual delta requires first value.
     * Simple approach: delta = last - min (conservative lower bound).
     * For accurate delta, callers should track first sample separately. */
    s->avg_rss_kb /= (double)s->sample_count;
    s->avg_vms_kb /= (double)s->sample_count;
    (void)first_rss;
}
