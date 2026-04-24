#include "cascade_policy.h"
#include <stddef.h>

void cascade_policy_apply(Cascade *c, CascadePolicyPreset preset,
                          size_t base_kb) {
    cascade_init(c);

    switch (preset) {
        case CASCADE_POLICY_CONSERVATIVE:
            cascade_add_level(c, base_kb,           5,  CASCADE_ACTION_LOG,      "warn");
            cascade_add_level(c, base_kb * 2,       3,  CASCADE_ACTION_ALERT,    "alert");
            cascade_add_level(c, base_kb * 4,       2,  CASCADE_ACTION_THROTTLE, "throttle");
            break;

        case CASCADE_POLICY_AGGRESSIVE:
            cascade_add_level(c, base_kb,           2,  CASCADE_ACTION_ALERT,    "alert");
            cascade_add_level(c, base_kb * 2,       1,  CASCADE_ACTION_THROTTLE, "throttle");
            cascade_add_level(c, base_kb * 3,       1,  CASCADE_ACTION_KILL,     "kill");
            break;

        case CASCADE_POLICY_MONITOR_ONLY:
            cascade_add_level(c, base_kb,           10, CASCADE_ACTION_LOG,      "info");
            cascade_add_level(c, base_kb * 2,       5,  CASCADE_ACTION_LOG,      "warn");
            cascade_add_level(c, base_kb * 4,       3,  CASCADE_ACTION_ALERT,    "alert");
            break;
    }
}

const char *cascade_policy_name(CascadePolicyPreset preset) {
    switch (preset) {
        case CASCADE_POLICY_CONSERVATIVE: return "conservative";
        case CASCADE_POLICY_AGGRESSIVE:   return "aggressive";
        case CASCADE_POLICY_MONITOR_ONLY: return "monitor_only";
        default:                          return "unknown";
    }
}
