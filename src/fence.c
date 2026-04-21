#include "fence.h"
#include <string.h>

void fence_init(fence_t *f, size_t soft_kb, size_t hard_kb)
{
    if (!f)
        return;
    memset(f, 0, sizeof(*f));
    f->soft_kb = soft_kb;
    f->hard_kb = hard_kb;
}

fence_status_t fence_check(fence_t *f, size_t current_kb)
{
    if (!f)
        return FENCE_OK;

    if (current_kb > f->peak_kb)
        f->peak_kb = current_kb;

    if (f->hard_kb > 0 && current_kb >= f->hard_kb) {
        f->breach_count++;
        return FENCE_BREACH;
    }

    if (f->soft_kb > 0 && current_kb >= f->soft_kb) {
        f->warn_count++;
        return FENCE_WARN;
    }

    return FENCE_OK;
}

const char *fence_status_str(fence_status_t status)
{
    switch (status) {
    case FENCE_OK:     return "ok";
    case FENCE_WARN:   return "warn";
    case FENCE_BREACH: return "breach";
    default:           return "unknown";
    }
}

void fence_reset(fence_t *f)
{
    if (!f)
        return;
    f->warn_count   = 0;
    f->breach_count = 0;
    f->peak_kb      = 0;
}
