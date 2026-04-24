#include "cascade.h"
#include <string.h>
#include <stddef.h>

void cascade_init(Cascade *c) {
    memset(c, 0, sizeof(*c));
    c->current_level = -1;
    c->ticks_at_level = 0;
    c->fired = false;
}

void cascade_add_level(Cascade *c, size_t threshold_kb, unsigned int sustain_ticks,
                       CascadeAction action, const char *label) {
    if (c->num_levels >= CASCADE_MAX_LEVELS)
        return;
    CascadeLevel *lvl = &c->levels[c->num_levels++];
    lvl->threshold_kb   = threshold_kb;
    lvl->sustain_ticks  = sustain_ticks;
    lvl->action         = action;
    lvl->label          = label;
}

/* Returns the highest matching level index for rss_kb, or -1 */
static int find_active_level(const Cascade *c, size_t rss_kb) {
    int best = -1;
    for (int i = 0; i < c->num_levels; i++) {
        if (rss_kb >= c->levels[i].threshold_kb)
            best = i;
    }
    return best;
}

CascadeAction cascade_update(Cascade *c, size_t rss_kb) {
    int active = find_active_level(c, rss_kb);

    if (active != c->current_level) {
        c->current_level  = active;
        c->ticks_at_level = 1;
        c->fired          = false;
    } else if (active >= 0) {
        c->ticks_at_level++;
    }

    if (active < 0)
        return CASCADE_ACTION_NONE;

    const CascadeLevel *lvl = &c->levels[active];

    if (!c->fired && c->ticks_at_level >= lvl->sustain_ticks) {
        c->fired = true;
        return lvl->action;
    }

    return CASCADE_ACTION_NONE;
}

void cascade_reset(Cascade *c) {
    c->current_level  = -1;
    c->ticks_at_level = 0;
    c->fired          = false;
}

int cascade_current_level(const Cascade *c) {
    return c->current_level;
}

const char *cascade_action_name(CascadeAction action) {
    switch (action) {
        case CASCADE_ACTION_NONE:     return "none";
        case CASCADE_ACTION_LOG:      return "log";
        case CASCADE_ACTION_ALERT:    return "alert";
        case CASCADE_ACTION_THROTTLE: return "throttle";
        case CASCADE_ACTION_KILL:     return "kill";
        default:                      return "unknown";
    }
}
