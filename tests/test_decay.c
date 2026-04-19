#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../src/decay.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  [TEST] %s\n", name); } while(0)
#define PASS()     do { tests_passed++; } while(0)
#define ASSERT_NEAR(a, b, eps) assert(fabs((a)-(b)) < (eps))

static void test_init_defaults(void) {
    TEST("decay_init sets alpha and marks uninitialized");
    decay_t d;
    decay_init(&d, 0.3);
    assert(d.initialized == 0);
    assert(d.alpha == 0.3);
    assert(decay_get(&d) == 0.0);
    PASS();
}

static void test_first_update_sets_value(void) {
    TEST("first decay_update seeds value directly");
    decay_t d;
    decay_init(&d, 0.5);
    double v = decay_update(&d, 100.0);
    assert(v == 100.0);
    assert(d.initialized == 1);
    PASS();
}

static void test_smoothing(void) {
    TEST("subsequent updates apply EMA smoothing");
    decay_t d;
    decay_init(&d, 0.5);
    decay_update(&d, 100.0);
    double v = decay_update(&d, 200.0);
    /* EMA: 0.5*200 + 0.5*100 = 150 */
    ASSERT_NEAR(v, 150.0, 1e-9);
    PASS();
}

static void test_alpha_one_no_smoothing(void) {
    TEST("alpha=1.0 returns raw value each time");
    decay_t d;
    decay_init(&d, 1.0);
    decay_update(&d, 50.0);
    double v = decay_update(&d, 999.0);
    ASSERT_NEAR(v, 999.0, 1e-9);
    PASS();
}

static void test_reset(void) {
    TEST("decay_reset clears initialized flag");
    decay_t d;
    decay_init(&d, 0.2);
    decay_update(&d, 42.0);
    decay_reset(&d);
    assert(d.initialized == 0);
    assert(decay_get(&d) == 0.0);
    PASS();
}

static void test_ema_oneshot(void) {
    TEST("decay_ema one-shot computation");
    double r = decay_ema(100.0, 200.0, 0.25);
    /* 0.25*200 + 0.75*100 = 125 */
    ASSERT_NEAR(r, 125.0, 1e-9);
    PASS();
}

int main(void) {
    printf("=== test_decay ===\n");
    test_init_defaults();
    test_first_update_sets_value();
    test_smoothing();
    test_alpha_one_no_smoothing();
    test_reset();
    test_ema_oneshot();
    printf("%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
