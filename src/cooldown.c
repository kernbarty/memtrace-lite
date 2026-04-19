#include "cooldown.h"
#include <string.h>

void cooldown_init(Cooldown *cd, uint64_t period_ms) {
    if (!cd) return;
    memset(cd, 0, sizeof(*cd));
    cd->period_ms = period_ms;
    cd->last_trigger_ms = 0;
    cd->active = false;
}

void cooldown_reset(Cooldown *cd) {
    if (!cd) return;
    cd->last_trigger_ms = 0;
    cd->active = false;
}

bool cooldown_trigger(Cooldown *cd, uint64_t now_ms) {
    if (!cd) return false;
    if (cd->active) {
        uint64_t elapsed = now_ms - cd->last_trigger_ms;
        if (elapsed < cd->period_ms)
            return false;
    }
    cd->last_trigger_ms = now_ms;
    cd->active = true;
    return true;
}

bool cooldown_ready(const Cooldown *cd, uint64_t now_ms) {
    if (!cd) return false;
    if (!cd->active) return true;
    return (now_ms - cd->last_trigger_ms) >= cd->period_ms;
}

uint64_t cooldown_remaining_ms(const Cooldown *cd, uint64_t now_ms) {
    if (!cd || !cd->active) return 0;
    uint64_t elapsed = now_ms - cd->last_trigger_ms;
    if (elapsed >= cd->period_ms) return 0;
    return cd->period_ms - elapsed;
}
