#include "quota.h"
#include <string.h>

void quota_init(quota_t *q, size_t soft_limit_kb, size_t hard_limit_kb) {
    if (!q) return;
    memset(q, 0, sizeof(*q));
    q->soft_limit_kb = soft_limit_kb;
    q->hard_limit_kb = hard_limit_kb;
}

void quota_reset(quota_t *q) {
    if (!q) return;
    q->current_kb = 0;
    q->exceed_count = 0;
    q->warn_count = 0;
}

quota_status_t quota_check(quota_t *q, size_t current_kb) {
    if (!q) return QUOTA_OK;
    q->current_kb = current_kb;

    if (q->hard_limit_kb > 0 && current_kb >= q->hard_limit_kb) {
        q->exceed_count++;
        return QUOTA_EXCEEDED;
    }
    if (q->soft_limit_kb > 0 && current_kb >= q->soft_limit_kb) {
        q->warn_count++;
        return QUOTA_WARN;
    }
    return QUOTA_OK;
}

quota_status_t quota_status(const quota_t *q) {
    if (!q) return QUOTA_OK;
    if (q->hard_limit_kb > 0 && q->current_kb >= q->hard_limit_kb)
        return QUOTA_EXCEEDED;
    if (q->soft_limit_kb > 0 && q->current_kb >= q->soft_limit_kb)
        return QUOTA_WARN;
    return QUOTA_OK;
}

const char *quota_status_str(quota_status_t status) {
    switch (status) {
        case QUOTA_OK:       return "ok";
        case QUOTA_WARN:     return "warn";
        case QUOTA_EXCEEDED: return "exceeded";
        default:             return "unknown";
    }
}

void quota_get_limits(const quota_t *q, size_t *soft_out, size_t *hard_out) {
    if (!q) return;
    if (soft_out) *soft_out = q->soft_limit_kb;
    if (hard_out) *hard_out = q->hard_limit_kb;
}
