#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "../src/cooldown.h"

static int passed = 0;
static int failed = 0;

#define TEST(name) printf("  %-40s", name)
#define PASS() do { puts("PASS"); passed++; } while(0)
#define FAIL(msg) do { printf("FAIL (%s)\n", msg); failed++; } while(0)

static void test_init(void) {
    TEST("cooldown_init sets interval");
    Cooldown cd;
    cooldown_init(&cd, 1.5);
    if (cd.interval_sec == 1.5 && !cd.armed) PASS(); else FAIL("wrong init");
}

static void test_first_check_passes(void) {
    TEST("first check always passes");
    Cooldown cd;
    cooldown_init(&cd, 10.0);
    if (cooldown_check(&cd)) PASS(); else FAIL("should pass");
}

static void test_second_check_blocked(void) {
    TEST("second check blocked within interval");
    Cooldown cd;
    cooldown_init(&cd, 10.0);
    cooldown_check(&cd);
    if (!cooldown_check(&cd)) PASS(); else FAIL("should be blocked");
}

static void test_remaining_nonzero(void) {
    TEST("remaining > 0 after trigger");
    Cooldown cd;
    cooldown_init(&cd, 5.0);
    cooldown_check(&cd);
    double r = cooldown_remaining(&cd);
    if (r > 0.0 && r <= 5.0) PASS(); else FAIL("bad remaining");
}

static void test_remaining_zero_before_trigger(void) {
    TEST("remaining 0 before any trigger");
    Cooldown cd;
    cooldown_init(&cd, 5.0);
    if (cooldown_remaining(&cd) == 0.0) PASS(); else FAIL("should be 0");
}

static void test_reset_allows_recheck(void) {
    TEST("reset allows immediate recheck");
    Cooldown cd;
    cooldown_init(&cd, 10.0);
    cooldown_check(&cd);
    cooldown_reset(&cd);
    if (cooldown_check(&cd)) PASS(); else FAIL("should pass after reset");
}

static void test_short_interval_expires(void) {
    TEST("short interval expires after sleep");
    Cooldown cd;
    cooldown_init(&cd, 0.05);
    cooldown_check(&cd);
    struct timespec ts = {0, 60000000L}; /* 60ms */
    nanosleep(&ts, NULL);
    if (cooldown_check(&cd)) PASS(); else FAIL("should expire");
}

int main(void) {
    printf("=== test_cooldown ===\n");
    test_init();
    test_first_check_passes();
    test_second_check_blocked();
    test_remaining_nonzero();
    test_remaining_zero_before_trigger();
    test_reset_allows_recheck();
    test_short_interval_expires();
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
