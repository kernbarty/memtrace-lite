#include <stdio.h>
#include <assert.h>
#include "../src/guard.h"

static void test_init() {
    Guard g;
    guard_init(&g, FENCE_POLICY_ABSOLUTE, 100, 200);
    assert(guard_is_armed(&g));
    assert(guard_breach_count(&g) == 0);
    printf("  [PASS] test_init\n");
}

static void test_no_breach() {
    Guard g;
    guard_init(&g, FENCE_POLICY_ABSOLUTE, 100, 200);
    FenceAction a = guard_check(&g, 50);
    assert(a == FENCE_ACTION_NONE);
    assert(guard_breach_count(&g) == 0);
    printf("  [PASS] test_no_breach\n");
}

static void test_soft_breach() {
    Guard g;
    guard_init(&g, FENCE_POLICY_ABSOLUTE, 100, 200);
    fence_policy_set_action(&g.policy, FENCE_ACTION_WARN);
    FenceAction a = guard_check(&g, 120);
    assert(a == FENCE_ACTION_WARN);
    assert(guard_breach_count(&g) == 1);
    printf("  [PASS] test_soft_breach\n");
}

static void test_hard_breach() {
    Guard g;
    guard_init(&g, FENCE_POLICY_ABSOLUTE, 100, 200);
    FenceAction a = guard_check(&g, 250);
    assert(a == FENCE_ACTION_KILL);
    assert(guard_breach_count(&g) == 1);
    printf("  [PASS] test_hard_breach\n");
}

static void test_breach_resets_on_clean() {
    Guard g;
    guard_init(&g, FENCE_POLICY_ABSOLUTE, 100, 200);
    fence_policy_set_action(&g.policy, FENCE_ACTION_WARN);
    guard_check(&g, 120);
    guard_check(&g, 130);
    assert(guard_breach_count(&g) == 2);
    guard_check(&g, 50);  /* clean sample */
    assert(guard_breach_count(&g) == 0);
    printf("  [PASS] test_breach_resets_on_clean\n");
}

static void test_disarmed_ignores_breach() {
    Guard g;
    guard_init(&g, FENCE_POLICY_ABSOLUTE, 100, 200);
    guard_disarm(&g);
    FenceAction a = guard_check(&g, 500);
    assert(a == FENCE_ACTION_NONE);
    assert(guard_breach_count(&g) == 0);
    printf("  [PASS] test_disarmed_ignores_breach\n");
}

static void test_rearm() {
    Guard g;
    guard_init(&g, FENCE_POLICY_ABSOLUTE, 100, 200);
    guard_disarm(&g);
    assert(!guard_is_armed(&g));
    guard_arm(&g);
    assert(guard_is_armed(&g));
    fence_policy_set_action(&g.policy, FENCE_ACTION_WARN);
    FenceAction a = guard_check(&g, 150);
    assert(a == FENCE_ACTION_WARN);
    printf("  [PASS] test_rearm\n");
}

static void test_reset() {
    Guard g;
    guard_init(&g, FENCE_POLICY_ABSOLUTE, 100, 200);
    fence_policy_set_action(&g.policy, FENCE_ACTION_WARN);
    guard_check(&g, 150);
    guard_check(&g, 160);
    guard_reset(&g);
    assert(guard_breach_count(&g) == 0);
    printf("  [PASS] test_reset\n");
}

int main(void) {
    printf("Running guard tests...\n");
    test_init();
    test_no_breach();
    test_soft_breach();
    test_hard_breach();
    test_breach_resets_on_clean();
    test_disarmed_ignores_breach();
    test_rearm();
    test_reset();
    printf("All guard tests passed.\n");
    return 0;
}
