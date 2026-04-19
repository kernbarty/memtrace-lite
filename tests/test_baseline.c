#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../src/baseline.h"

static void test_init(void) {
    Baseline b;
    baseline_init(&b);
    assert(b.count == 0);
    assert(!baseline_is_ready(&b));
    assert(baseline_mean(&b) == 0.0);
    printf("  test_init passed\n");
}

static void test_not_ready_below_min(void) {
    Baseline b;
    baseline_init(&b);
    for (int i = 0; i < 5; i++)
        baseline_add(&b, 100.0, (time_t)(1000 + i));
    assert(!baseline_is_ready(&b));
    printf("  test_not_ready_below_min passed\n");
}

static void test_ready_after_min_samples(void) {
    Baseline b;
    baseline_init(&b);
    for (int i = 0; i < 10; i++)
        baseline_add(&b, 200.0 + i, (time_t)(2000 + i));
    assert(baseline_is_ready(&b));
    printf("  test_ready_after_min_samples passed\n");
}

static void test_mean_accuracy(void) {
    Baseline b;
    baseline_init(&b);
    for (int i = 0; i < 10; i++)
        baseline_add(&b, 100.0, (time_t)(3000 + i));
    double m = baseline_mean(&b);
    assert(fabs(m - 100.0) < 1e-6);
    printf("  test_mean_accuracy passed\n");
}

static void test_deviation(void) {
    Baseline b;
    baseline_init(&b);
    /* Alternating 90 and 110 => mean=100, stddev~10 */
    for (int i = 0; i < 20; i++)
        baseline_add(&b, (i % 2 == 0) ? 90.0 : 110.0, (time_t)(4000 + i));
    assert(baseline_is_ready(&b));
    double dev = baseline_deviation(&b, 100.0);
    assert(fabs(dev) < 0.1);
    double dev2 = baseline_deviation(&b, 120.0);
    assert(dev2 > 1.5);
    printf("  test_deviation passed\n");
}

static void test_reset(void) {
    Baseline b;
    baseline_init(&b);
    for (int i = 0; i < 20; i++)
        baseline_add(&b, 50.0, (time_t)(5000 + i));
    assert(baseline_is_ready(&b));
    baseline_reset(&b);
    assert(!baseline_is_ready(&b));
    assert(b.count == 0);
    printf("  test_reset passed\n");
}

static void test_ring_overflow(void) {
    Baseline b;
    baseline_init(&b);
    for (int i = 0; i < 80; i++)
        baseline_add(&b, 300.0, (time_t)(6000 + i));
    assert(b.count == BASELINE_MAX_SAMPLES);
    assert(fabs(baseline_mean(&b) - 300.0) < 1e-6);
    printf("  test_ring_overflow passed\n");
}

int main(void) {
    printf("Running baseline tests...\n");
    test_init();
    test_not_ready_below_min();
    test_ready_after_min_samples();
    test_mean_accuracy();
    test_deviation();
    test_reset();
    test_ring_overflow();
    printf("All baseline tests passed.\n");
    return 0;
}
