#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../src/cooldown.h"

static int passed = 0;
static int failed = 0;

#define TEST(name) static void test_##name(void)
#define RUN(name) do { test_##name(); printf("  [PASS] " #name "\n"); passed++; } while(0)

TEST(init) {
    Cooldown cd;
    cooldown_init(&cd, 500);
    assert(cd.period_ms == 500);
    assert(!cd.active);
    assert(cd.last_trigger_ms == 0);
}

TEST(ready_when_inactive) {
    Cooldown cd;
    cooldown_init(&cd, 1000);
    assert(cooldown_ready(&cd, 9999));
}

TEST(trigger_first_time) {
    Cooldown cd;
    cooldown_init(&cd, 1000);
    bool ok = cooldown_trigger(&cd, 100);
    assert(ok);
    assert(cd.active);
    assert(cd.last_trigger_ms == 100);
}

TEST(trigger_blocked_during_cooldown) {
    Cooldown cd;
    cooldown_init(&cd, 1000);
    cooldown_trigger(&cd, 0);
    bool ok = cooldown_trigger(&cd, 500);
    assert(!ok);
}

TEST(trigger_allowed_after_period) {
    Cooldown cd;
    cooldown_init(&cd, 1000);
    cooldown_trigger(&cd, 0);
    bool ok = cooldown_trigger(&cd, 1000);
    assert(ok);
    assert(cd.last_trigger_ms == 1000);
}

TEST(remaining_ms) {
    Cooldown cd;
    cooldown_init(&cd, 1000);
    cooldown_trigger(&cd, 200);
    uint64_t rem = cooldown_remaining_ms(&cd, 700);
    assert(rem == 500);
}

TEST(remaining_ms_expired) {
    Cooldown cd;
    cooldown_init(&cd, 1000);
    cooldown_trigger(&cd, 0);
    assert(cooldown_remaining_ms(&cd, 1500) == 0);
}

TEST(remaining_ms_inactive) {
    /* When cooldown is not active, remaining should be 0 regardless of time */
    Cooldown cd;
    cooldown_init(&cd, 1000);
    assert(cooldown_remaining_ms(&cd, 0) == 0);
    assert(cooldown_remaining_ms(&cd, 9999) == 0);
}

TEST(reset) {
    Cooldown cd;
    cooldown_init(&cd, 500);
    cooldown_trigger(&cd, 100);
    cooldown_reset(&cd);
    assert(!cd.active);
    assert(cooldown_ready(&cd, 0));
}

int main(void) {
    printf("Running cooldown tests...\n");
    RUN(init);
    RUN(ready_when_inactive);
    RUN(trigger_first_time);
    RUN(trigger_blocked_during_cooldown);
    RUN(trigger_allowed_after_period);
    RUN(remaining_ms);
    RUN(remaining_ms_expired);
    RUN(remaining_ms_inactive);
    RUN(reset);
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
