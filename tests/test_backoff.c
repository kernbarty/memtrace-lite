#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "../src/backoff.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  test: %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("OK\n"); } while(0)
#define FAIL(msg)  do { printf("FAIL: %s\n", msg); } while(0)

static void test_init(void) {
    TEST("backoff_init sets fields correctly");
    backoff_t b;
    backoff_init(&b, 10, 1000, false);
    assert(b.min_ms == 10);
    assert(b.max_ms == 1000);
    assert(b.current_ms == 10);
    assert(b.attempts == 0);
    assert(b.jitter == false);
    PASS();
}

static void test_next_doubles(void) {
    TEST("backoff_next doubles delay");
    backoff_t b;
    backoff_init(&b, 10, 10000, false);
    uint32_t d1 = backoff_next(&b);
    uint32_t d2 = backoff_next(&b);
    uint32_t d3 = backoff_next(&b);
    assert(d1 == 20);
    assert(d2 == 40);
    assert(d3 == 80);
    assert(b.attempts == 3);
    PASS();
}

static void test_cap_at_max(void) {
    TEST("backoff_next caps at max_ms");
    backoff_t b;
    backoff_init(&b, 100, 200, false);
    backoff_next(&b); /* 200 */
    uint32_t d = backoff_next(&b); /* still 200 */
    assert(d == 200);
    assert(backoff_saturated(&b));
    PASS();
}

static void test_reset(void) {
    TEST("backoff_reset restores initial state");
    backoff_t b;
    backoff_init(&b, 50, 800, false);
    backoff_next(&b);
    backoff_next(&b);
    backoff_reset(&b);
    assert(b.current_ms == 50);
    assert(b.attempts == 0);
    assert(!backoff_saturated(&b));
    PASS();
}

static void test_jitter_range(void) {
    TEST("backoff_next with jitter stays within [min, max]");
    backoff_t b;
    backoff_init(&b, 10, 5000, true);
    for (int i = 0; i < 20; i++) {
        uint32_t d = backoff_next(&b);
        assert(d >= b.min_ms);
        assert(d <= b.max_ms);
    }
    PASS();
}

static void test_current_no_advance(void) {
    TEST("backoff_current does not advance state");
    backoff_t b;
    backoff_init(&b, 10, 1000, false);
    uint32_t c1 = backoff_current(&b);
    uint32_t c2 = backoff_current(&b);
    assert(c1 == c2);
    assert(b.attempts == 0);
    PASS();
}

int main(void) {
    printf("Running backoff tests...\n");
    test_init();
    test_next_doubles();
    test_cap_at_max();
    test_reset();
    test_jitter_range();
    test_current_no_advance();
    printf("%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
