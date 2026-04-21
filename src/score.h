#ifndef SCORE_H
#define SCORE_H

/*
 * Compute a composite memory health score in [0.0, 1.0].
 * 1.0 = perfectly healthy, 0.0 = critically bad.
 *
 * Inputs:
 *   rss_kb       - current RSS in KB
 *   peak_rss_kb  - peak RSS ever observed for this pid
 *   quota_kb     - configured memory quota in KB (0 = unlimited)
 *   growth_rate  - recent growth rate in KB/s (from rate module)
 *   pressure     - system memory pressure in [0.0, 1.0]
 */
float score_compute(long rss_kb, long peak_rss_kb,
                    long quota_kb, float growth_rate,
                    float pressure);

/*
 * Map a raw score to a human-readable tier string.
 * Returns one of: "healthy", "warning", "critical"
 */
const char *score_tier(float score);

#endif /* SCORE_H */
