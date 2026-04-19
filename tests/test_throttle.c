#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "../src/throttle.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond, msg) do { \
    if (cond) { printf("  PASS: %s\n", msg); passed++; } \
    else       { printf("  FAIL: %s\n", msg); failed++; } \
} while(0)

static void test_basic_allow(void) {
    printf("test_basic_allow\n");
    Throttle t;
    throttle_init(&t, 100);
    /* First call always allowed */
    CHECK(throttle_allow(&t), "first event allowed");
    /* Immediate second call throttled */
    CHECK(!throttle_allow(&t), "second event suppressed");
    CHECK(throttle_suppressed(&t) == 1, "suppressed count is 1");
}

static void test_disabled(void) {
    printf("test_disabled\n");
    Throttle t;
    throttle_init(&t, 50);
    throttle_set_enabled(&t, false);
    CHECK(throttle_allow(&t), "allowed when disabled");
    CHECK(throttle_allow(&t), "second allowed when disabled");
    CHECK(throttle_suppressed(&t) == 0, "no suppression when disabled");
}

static void test_reset(void) {
    printf("test_reset\n");
    Throttle t;
    throttle_init(&t, 200);
    throttle_allow(&t);
    throttle_allow(&t); /* suppressed */
    CHECK(throttle_suppressed(&t) == 1, "one suppressed before reset");
    throttle_reset(&t);
    CHECK(throttle_suppressed(&t) == 0, "suppressed cleared after reset");
    CHECK(throttle_allow(&t), "allowed after reset");
}

static void test_interval_expiry(void) {
    printf("test_interval_expiry\n");
    Throttle t;
    throttle_init(&t, 50); /* 50 ms */
    CHECK(throttle_allow(&t), "first allowed");
    CHECK(!throttle_allow(&t), "suppressed before interval");
    usleep(60 * 1000); /* wait 60ms */
    CHECK(throttle_allow(&t), "allowed after interval elapsed");
}

static void test_null_safety(void) {
    printf("test_null_safety\n");
    CHECK(throttle_allow(NULL), "null returns allow");
    CHECK(throttle_suppressed(NULL) == 0, "null suppressed is 0");
    throttle_reset(NULL);
    throttle_set_enabled(NULL, true);
    passed++; printf("  PASS: null ops do not crash\n");
}

int main(void) {
    printf("=== test_throttle ===\n");
    test_basic_allow();
    test_disabled();
    test_reset();
    test_interval_expiry();
    test_null_safety();
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
