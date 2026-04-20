#include "escalate.h"
#include <string.h>

void escalate_init(escalate_t *e, const escalate_config_t *cfg) {
    memset(e, 0, sizeof(*e));
    e->cfg = *cfg;
    e->current_level = ESCALATE_LEVEL_NONE;
    e->trigger_count = 0;
    e->last_trigger_time = 0;
}

void escalate_reset(escalate_t *e) {
    e->current_level = ESCALATE_LEVEL_NONE;
    e->trigger_count = 0;
    e->last_trigger_time = 0;
}

escalate_level_t escalate_update(escalate_t *e, int triggered, uint64_t now_ms) {
    if (!triggered) {
        if (e->cfg.auto_reset_ms > 0 &&
            e->last_trigger_time > 0 &&
            (now_ms - e->last_trigger_time) >= e->cfg.auto_reset_ms) {
            escalate_reset(e);
        }
        return e->current_level;
    }

    e->trigger_count++;
    e->last_trigger_time = now_ms;

    if (e->trigger_count >= e->cfg.critical_threshold) {
        e->current_level = ESCALATE_LEVEL_CRITICAL;
    } else if (e->trigger_count >= e->cfg.warning_threshold) {
        e->current_level = ESCALATE_LEVEL_WARNING;
    } else if (e->trigger_count >= e->cfg.notice_threshold) {
        e->current_level = ESCALATE_LEVEL_NOTICE;
    } else {
        e->current_level = ESCALATE_LEVEL_NONE;
    }

    return e->current_level;
}

escalate_level_t escalate_current_level(const escalate_t *e) {
    return e->current_level;
}

uint32_t escalate_trigger_count(const escalate_t *e) {
    return e->trigger_count;
}

const char *escalate_level_name(escalate_level_t level) {
    switch (level) {
        case ESCALATE_LEVEL_NONE:     return "none";
        case ESCALATE_LEVEL_NOTICE:   return "notice";
        case ESCALATE_LEVEL_WARNING:  return "warning";
        case ESCALATE_LEVEL_CRITICAL: return "critical";
        default:                      return "unknown";
    }
}
