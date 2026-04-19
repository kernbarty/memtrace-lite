#include "budget_policy.h"
#include <string.h>

void budget_policy_init(budget_policy_t *p,
                        policy_action_t on_warn,
                        policy_action_t on_exceed,
                        bool auto_reset_on_ok) {
    p->on_warn = on_warn;
    p->on_exceed = on_exceed;
    p->auto_reset_on_ok = auto_reset_on_ok;
}

policy_action_t budget_policy_evaluate(const budget_policy_t *p,
                                        budget_t *b,
                                        budget_status_t status) {
    switch (status) {
        case BUDGET_OK:
            if (p->auto_reset_on_ok)
                budget_reset(b);
            return POLICY_ACTION_NONE;
        case BUDGET_WARN:
            return p->on_warn;
        case BUDGET_EXCEEDED:
            return p->on_exceed;
    }
    return POLICY_ACTION_NONE;
}

const char *budget_policy_action_str(policy_action_t action) {
    switch (action) {
        case POLICY_ACTION_NONE:  return "none";
        case POLICY_ACTION_LOG:   return "log";
        case POLICY_ACTION_ALERT: return "alert";
        case POLICY_ACTION_KILL:  return "kill";
    }
    return "unknown";
}
