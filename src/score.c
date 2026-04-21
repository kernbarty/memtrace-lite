#include "score.h"
#include <math.h>

/* Weight constants — must sum to 1.0 */
#define W_QUOTA    0.40f
#define W_PEAK     0.25f
#define W_GROWTH   0.20f
#define W_PRESSURE 0.15f

/* Clamp a float to [0,1] */
static float clampf(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

float score_compute(long rss_kb, long peak_rss_kb,
                    long quota_kb, float growth_rate,
                    float pressure) {
    float quota_score = 1.0f;
    if (quota_kb > 0) {
        float ratio = (float)rss_kb / (float)quota_kb;
        /* Linear penalty: at 100% usage score = 0 */
        quota_score = clampf(1.0f - ratio);
    }

    float peak_score = 1.0f;
    if (peak_rss_kb > 0) {
        float ratio = (float)rss_kb / (float)peak_rss_kb;
        /* Penalise if current is close to or above historical peak */
        peak_score = clampf(1.0f - (ratio - 0.5f) * 2.0f);
    }

    /* growth_rate penalty: 0 KB/s => 1.0, 1024 KB/s => 0.0 */
    float growth_score = 1.0f;
    if (growth_rate > 0.0f) {
        growth_score = clampf(1.0f - growth_rate / 1024.0f);
    }

    float pressure_score = clampf(1.0f - pressure);

    float composite = W_QUOTA    * quota_score
                    + W_PEAK     * peak_score
                    + W_GROWTH   * growth_score
                    + W_PRESSURE * pressure_score;

    return clampf(composite);
}

const char *score_tier(float score) {
    if (score >= 0.65f) return "healthy";
    if (score >= 0.35f) return "warning";
    return "critical";
}
