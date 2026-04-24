#include <stdio.h>
#include <assert.h>
#include "../src/cascade.h"
#include "../src/cascade_policy.h"

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; } \
    else { fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg); } \
} while(0)

static void test_init(void) {
    Cascade c;
    cascade_init(&c);
    CHECK(c.num_levels == 0,    "num_levels starts at 0");
    CHECK(c.current_level == -1, "current_level starts at -1");
    CHECK(c.fired == false,     "fired starts false");
}

static void test_single_level_fires_after_sustain(void) {
    Cascade c;
    cascade_init(&c);
    cascade_add_level(&c, 100, 3, CASCADE_ACTION_LOG, "warn");

    /* Below threshold — no action */
    CHECK(cascade_update(&c, 50) == CASCADE_ACTION_NONE, "below threshold");

    /* At threshold but not yet sustained */
    CHECK(cascade_update(&c, 150) == CASCADE_ACTION_NONE, "tick 1");
    CHECK(cascade_update(&c, 150) == CASCADE_ACTION_NONE, "tick 2");

    /* Third tick meets sustain requirement */
    CHECK(cascade_update(&c, 150) == CASCADE_ACTION_LOG,  "fires at tick 3");

    /* Does not fire again (already fired) */
    CHECK(cascade_update(&c, 150) == CASCADE_ACTION_NONE, "no double-fire");
}

static void test_level_escalation(void) {
    Cascade c;
    cascade_init(&c);
    cascade_add_level(&c, 100, 1, CASCADE_ACTION_LOG,   "warn");
    cascade_add_level(&c, 200, 1, CASCADE_ACTION_ALERT, "alert");

    CHECK(cascade_update(&c, 150) == CASCADE_ACTION_LOG,   "level 0 fires");
    CHECK(cascade_update(&c, 250) == CASCADE_ACTION_ALERT, "level 1 fires after escalation");
    CHECK(cascade_current_level(&c) == 1, "current level is 1");
}

static void test_reset(void) {
    Cascade c;
    cascade_init(&c);
    cascade_add_level(&c, 100, 1, CASCADE_ACTION_LOG, "warn");
    cascade_update(&c, 200);
    cascade_reset(&c);
    CHECK(c.current_level == -1, "reset clears level");
    CHECK(c.fired == false,      "reset clears fired");
    /* Should fire again after reset */
    CHECK(cascade_update(&c, 200) == CASCADE_ACTION_LOG, "fires again after reset");
}

static void test_policy_conservative(void) {
    Cascade c;
    cascade_policy_apply(&c, CASCADE_POLICY_CONSERVATIVE, 256);
    CHECK(c.num_levels == 3, "conservative has 3 levels");
    CHECK(c.levels[0].action == CASCADE_ACTION_LOG,      "level 0 is log");
    CHECK(c.levels[1].action == CASCADE_ACTION_ALERT,    "level 1 is alert");
    CHECK(c.levels[2].action == CASCADE_ACTION_THROTTLE, "level 2 is throttle");
}

static void test_policy_aggressive(void) {
    Cascade c;
    cascade_policy_apply(&c, CASCADE_POLICY_AGGRESSIVE, 512);
    CHECK(c.num_levels == 3, "aggressive has 3 levels");
    CHECK(c.levels[2].action == CASCADE_ACTION_KILL, "top level is kill");
}

static void test_action_name(void) {
    CHECK(cascade_action_name(CASCADE_ACTION_NONE)     != NULL, "none name");
    CHECK(cascade_action_name(CASCADE_ACTION_THROTTLE) != NULL, "throttle name");
    CHECK(cascade_action_name(CASCADE_ACTION_KILL)     != NULL, "kill name");
}

int main(void) {
    test_init();
    test_single_level_fires_after_sustain();
    test_level_escalation();
    test_reset();
    test_policy_conservative();
    test_policy_aggressive();
    test_action_name();

    printf("cascade: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
