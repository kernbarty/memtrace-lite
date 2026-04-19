#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../src/rate.h"

static void test_init() {
    MemRate r;
    rate_init(&r);
    assert(r.valid == 0);
    assert(rate_get(&r) == 0.0);
    printf("PASS test_init\n");
}

static void test_single_sample_no_rate() {
    MemRate r;
    rate_init(&r);
    rate_update(&r, 1024);
    assert(rate_get(&r) == 0.0);
    printf("PASS test_single_sample_no_rate\n");
}

static void test_two_samples_rate() {
    MemRate r;
    rate_init(&r);
    rate_update(&r, 1000);
    /* simulate second sample after a tiny sleep */
    struct timespec ts = {0, 100000000}; /* 100ms */
    nanosleep(&ts, NULL);
    rate_update(&r, 1100); /* +100 KB in ~0.1s => ~1000 KB/s */
    double v = rate_get(&r);
    assert(v > 0.0);
    printf("PASS test_two_samples_rate (rate=%.1f KB/s)\n", v);
}

static void test_exceeds() {
    MemRate r;
    rate_init(&r);
    rate_update(&r, 0);
    struct timespec ts = {0, 100000000};
    nanosleep(&ts, NULL);
    rate_update(&r, 500);
    assert(rate_exceeds(&r, 100.0) == 1);
    assert(rate_exceeds(&r, 100000.0) == 0);
    printf("PASS test_exceeds\n");
}

static void test_format() {
    MemRate r;
    rate_init(&r);
    rate_update(&r, 200);
    struct timespec ts = {0, 100000000};
    nanosleep(&ts, NULL);
    rate_update(&r, 300);
    char buf[64];
    rate_format(&r, buf, sizeof(buf));
    assert(buf[0] == '+' || buf[0] == '-');
    printf("PASS test_format: %s\n", buf);
}

static void test_shrink_rate() {
    MemRate r;
    rate_init(&r);
    rate_update(&r, 2000);
    struct timespec ts = {0, 100000000};
    nanosleep(&ts, NULL);
    rate_update(&r, 1800);
    double v = rate_get(&r);
    assert(v < 0.0);
    printf("PASS test_shrink_rate (rate=%.1f KB/s)\n", v);
}

int main(void) {
    test_init();
    test_single_sample_no_rate();
    test_two_samples_rate();
    test_exceeds();
    test_format();
    test_shrink_rate();
    printf("All rate tests passed.\n");
    return 0;
}
