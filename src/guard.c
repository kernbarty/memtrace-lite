#include "guard.h"
#include <string.h>

void guard_init(Guard *g, FencePolicyType type,
                size_t soft_limit, size_t hard_limit) {
    if (!g) return;
    memset(g, 0, sizeof(*g));
    fence_policy_init(&g->policy, type, soft_limit, hard_limit);
    g->armed = 1;
}

void guard_arm(Guard *g) {
    if (g) g->armed = 1;
}

void guard_disarm(Guard *g) {
    if (g) g->armed = 0;
}

FenceAction guard_check(Guard *g, size_t value) {
    if (!g || !g->armed) return FENCE_ACTION_NONE;

    g->last_value = value;
    FenceAction action = fence_policy_evaluate(&g->policy, value);

    if (action == FENCE_ACTION_KILL) {
        g->hard_breach_count++;
        g->breach_count++;
    } else if (action != FENCE_ACTION_NONE) {
        g->breach_count++;
    } else {
        /* reset consecutive counter on clean sample */
        g->breach_count = 0;
    }

    return action;
}

void guard_reset(Guard *g) {
    if (!g) return;
    g->breach_count      = 0;
    g->hard_breach_count = 0;
    g->last_value        = 0;
}

size_t guard_breach_count(const Guard *g) {
    return g ? g->breach_count : 0;
}

int guard_is_armed(const Guard *g) {
    return g ? g->armed : 0;
}
