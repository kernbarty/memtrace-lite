#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../src/trend.h"

static void test_stable(void) {
    uint64_t samples[] = {1024, 1025, 1023, 1024, 1026};
    trend_result_t r = trend_analyze(samples, 5);
    assert(r.direction == TREND_STABLE);
    assert(r.sample_count == 5);
    printf("[PASS] test_stable: slope=%.2f\n", r.slope_kb_per_sample);
}

static void test_growing(void) {
    uint64_t samples[] = {1000, 1050, 1100, 1150, 1200, 1250};
    trend_result_t r = trend_analyze(samples, 6);
    assert(r.direction == TREND_GROWING);
    assert(r.slope_kb_per_sample > 10.0);
    printf("[PASS] test_growing: slope=%.2f\n", r.slope_kb_per_sample);
}

static void test_shrinking(void) {
    uint64_t samples[] = {2000, 1950, 1900, 1850, 1800};
    trend_result_t r = trend_analyze(samples, 5);
    assert(r.direction == TREND_SHRINKING);
    assert(r.slope_kb_per_sample < -10.0);
    printf("[PASS] test_shrinking: slope=%.2f\n", r.slope_kb_per_sample);
}

static void test_volatile(void) {
    uint64_t samples[] = {500, 2000, 300, 1800, 400, 2100};
    trend_result_t r = trend_analyze(samples, 6);
    assert(r.direction == TREND_VOLATILE);
    printf("[PASS] test_volatile: variance=%.2f\n", r.variance);
}

static void test_single_sample(void) {
    uint64_t samples[] = {1024};
    trend_result_t r = trend_analyze(samples, 1);
    assert(r.direction == TREND_STABLE);
    assert(r.sample_count == 1);
    printf("[PASS] test_single_sample\n");
}

static void test_direction_str(void) {
    assert(__builtin_strcmp(trend_direction_str(TREND_STABLE),    "stable")    == 0);
    assert(__builtin_strcmp(trend_direction_str(TREND_GROWING),   "growing")   == 0);
    assert(__builtin_strcmp(trend_direction_str(TREND_SHRINKING), "shrinking") == 0);
    assert(__builtin_strcmp(trend_direction_str(TREND_VOLATILE),  "volatile")  == 0);
    printf("[PASS] test_direction_str\n");
}

int main(void) {
    test_stable();
    test_growing();
    test_shrinking();
    test_volatile();
    test_single_sample();
    test_direction_str();
    printf("All trend tests passed.\n");
    return 0;
}
