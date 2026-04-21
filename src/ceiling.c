#include "ceiling.h"
#include <string.h>

void ceiling_init(Ceiling *c, const CeilingConfig *cfg) {
    memset(c, 0, sizeof(*c));
    c->cfg = *cfg;
    c->active = false;
    c->breach_count = 0;
    c->consecutive = 0;
}

void ceiling_reset(Ceiling *c) {
    c->active = false;
    c->breach_count = 0;
    c->consecutive = 0;
    c->last_breach_value = 0;
}

CeilingResult ceiling_check(Ceiling *c, long value) {
    CeilingResult result;
    result.value = value;
    result.limit = c->cfg.hard_limit;
    result.breached = false;
    result.consecutive = 0;
    result.severity = CEILING_OK;

    if (value >= c->cfg.hard_limit) {
        c->breach_count++;
        c->consecutive++;
        c->active = true;
        c->last_breach_value = value;
        result.breached = true;
        result.consecutive = c->consecutive;
        result.severity = CEILING_HARD;
    } else if (c->cfg.soft_limit > 0 && value >= c->cfg.soft_limit) {
        c->consecutive = 0;
        result.breached = true;
        result.severity = CEILING_SOFT;
    } else {
        c->consecutive = 0;
        if (c->active && value < c->cfg.hard_limit) {
            c->active = false;
        }
    }

    return result;
}

bool ceiling_is_active(const Ceiling *c) {
    return c->active;
}

unsigned int ceiling_breach_count(const Ceiling *c) {
    return c->breach_count;
}

const char *ceiling_severity_str(CeilingSeverity sev) {
    switch (sev) {
        case CEILING_OK:   return "ok";
        case CEILING_SOFT: return "soft";
        case CEILING_HARD: return "hard";
        default:           return "unknown";
    }
}
