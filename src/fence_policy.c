#include "fence_policy.h"
#include <string.h>

void fence_policy_init(FencePolicy *policy, FencePolicyType type,
                       size_t soft_limit, size_t hard_limit) {
    if (!policy) return;
    memset(policy, 0, sizeof(*policy));
    policy->type       = type;
    policy->soft_limit = soft_limit;
    policy->hard_limit = hard_limit;
    policy->action     = FENCE_ACTION_WARN;
}

FenceAction fence_policy_evaluate(const FencePolicy *policy, size_t value) {
    if (!policy) return FENCE_ACTION_NONE;

    if (policy->hard_limit > 0 && value >= policy->hard_limit) {
        return FENCE_ACTION_KILL;
    }
    if (policy->soft_limit > 0 && value >= policy->soft_limit) {
        return policy->action;
    }
    return FENCE_ACTION_NONE;
}

const char *fence_policy_action_str(FenceAction action) {
    switch (action) {
        case FENCE_ACTION_NONE:    return "none";
        case FENCE_ACTION_WARN:    return "warn";
        case FENCE_ACTION_THROTTLE: return "throttle";
        case FENCE_ACTION_KILL:    return "kill";
        default:                   return "unknown";
    }
}

void fence_policy_set_action(FencePolicy *policy, FenceAction action) {
    if (!policy) return;
    policy->action = action;
}
