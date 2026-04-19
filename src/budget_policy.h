#ifndef BUDGET_POLICY_H
#define BUDGET_POLICY_H

#include "budget.h"
#include <stdbool.h>

typedef enum {
    POLICY_ACTION_NONE,
    POLICY_ACTION_LOG,
    POLICY_ACTION_ALERT,
    POLICY_ACTION_KILL
} policy_action_t;

typedef struct {
    policy_action_t on_warn;
    policy_action_t on_exceed;
    bool auto_reset_on_ok;
} budget_policy_t;

void budget_policy_init(budget_policy_t *p,
                        policy_action_t on_warn,
                        policy_action_t on_exceed,
                        bool auto_reset_on_ok);

policy_action_t budget_policy_evaluate(const budget_policy_t *p,
                                        budget_t *b,
                                        budget_status_t status);

const char *budget_policy_action_str(policy_action_t action);

#endif
