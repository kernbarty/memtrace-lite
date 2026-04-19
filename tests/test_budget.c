#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/budget.h"
#include "../src/budget_policy.h"

static void test_budget_init() {
    budget_t b;
    budget_init(&b, 1024, 768);
    assert(b.limit_kb == 1024);
    assert(b.warn_threshold_kb == 768);
    assert(b.current_kb == 0);
    assert(b.peak_kb == 0);
    assert(b.exceed_count == 0);
    printf("PASS test_budget_init\n");
}

static void test_budget_update_ok() {
    budget_t b;
    budget_init(&b, 1024, 768);
    budget_status_t s = budget_update(&b, 512);
    assert(s == BUDGET_OK);
    assert(b.current_kb == 512);
    assert(b.peak_kb == 512);
    printf("PASS test_budget_update_ok\n");
}

static void test_budget_update_warn() {
    budget_t b;
    budget_init(&b, 1024, 768);
    budget_status_t s = budget_update(&b, 800);
    assert(s == BUDGET_WARN);
    printf("PASS test_budget_update_warn\n");
}

static void test_budget_update_exceeded() {
    budget_t b;
    budget_init(&b, 1024, 768);
    budget_status_t s = budget_update(&b, 1100);
    assert(s == BUDGET_EXCEEDED);
    assert(b.exceed_count == 1);
    assert(budget_is_exceeded(&b));
    printf("PASS test_budget_update_exceeded\n");
}

static void test_budget_peak_tracking() {
    budget_t b;
    budget_init(&b, 2048, 1024);
    budget_update(&b, 500);
    budget_update(&b, 900);
    budget_update(&b, 700);
    assert(b.peak_kb == 900);
    printf("PASS test_budget_peak_tracking\n");
}

static void test_budget_usage_ratio() {
    budget_t b;
    budget_init(&b, 1000, 800);
    budget_update(&b, 500);
    float r = budget_usage_ratio(&b);
    assert(r > 0.49f && r < 0.51f);
    printf("PASS test_budget_usage_ratio\n");
}

static void test_policy_evaluate() {
    budget_t b;
    budget_policy_t p;
    budget_init(&b, 1024, 768);
    budget_policy_init(&p, POLICY_ACTION_LOG, POLICY_ACTION_ALERT, false);

    budget_status_t s = budget_update(&b, 800);
    policy_action_t a = budget_policy_evaluate(&p, &b, s);
    assert(a == POLICY_ACTION_LOG);

    s = budget_update(&b, 1100);
    a = budget_policy_evaluate(&p, &b, s);
    assert(a == POLICY_ACTION_ALERT);
    printf("PASS test_policy_evaluate\n");
}

static void test_policy_auto_reset() {
    budget_t b;
    budget_policy_t p;
    budget_init(&b, 1024, 768);
    budget_policy_init(&p, POLICY_ACTION_NONE, POLICY_ACTION_NONE, true);
    budget_update(&b, 900);
    assert(b.peak_kb == 900);
    budget_status_t s = budget_update(&b, 100);
    budget_policy_evaluate(&p, &b, s);
    assert(b.peak_kb == 0);
    printf("PASS test_policy_auto_reset\n");
}

int main(void) {
    test_budget_init();
    test_budget_update_ok();
    test_budget_update_warn();
    test_budget_update_exceeded();
    test_budget_peak_tracking();
    test_budget_usage_ratio();
    test_policy_evaluate();
    test_policy_auto_reset();
    printf("All budget tests passed.\n");
    return 0;
}
