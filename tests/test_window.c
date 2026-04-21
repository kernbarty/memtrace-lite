#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../src/window.h"

static void test_init(void) {
    window_t w;
    assert(window_init(&w, 8, 60) == 0);
    assert(w.capacity == 8);
    assert(w.count == 0);
    assert(w.duration_s == 60);
    printf("PASS test_init\n");
}

static void test_push_and_count(void) {
    window_t w;
    window_init(&w, 4, 60);
    time_t now = 1000;

    window_push(&w, 100, now);
    window_push(&w, 200, now + 1);
    window_push(&w, 300, now + 2);

    assert(window_count(&w, now + 2) == 3);
    printf("PASS test_push_and_count\n");
}

static void test_aggregate_basic(void) {
    window_t w;
    window_init(&w, 8, 120);
    time_t now = 2000;

    window_push(&w, 10, now);
    window_push(&w, 20, now + 1);
    window_push(&w, 30, now + 2);

    window_stats_t s = window_aggregate(&w, now + 2);
    assert(s.count == 3);
    assert(s.min == 10);
    assert(s.max == 30);
    assert(s.sum == 60);
    assert(s.avg > 19.9 && s.avg < 20.1);
    printf("PASS test_aggregate_basic\n");
}

static void test_expiry(void) {
    window_t w;
    window_init(&w, 8, 10); /* 10-second window */
    time_t now = 3000;

    window_push(&w, 50, now);       /* will expire */
    window_push(&w, 60, now + 5);   /* will expire */
    window_push(&w, 70, now + 11);  /* still valid */
    window_push(&w, 80, now + 12);  /* still valid */

    /* Query at now+12: entries at now and now+5 are older than 10s */
    size_t cnt = window_count(&w, now + 12);
    assert(cnt == 2);

    window_stats_t s = window_aggregate(&w, now + 12);
    assert(s.min == 70);
    assert(s.max == 80);
    printf("PASS test_expiry\n");
}

static void test_capacity_wrap(void) {
    window_t w;
    window_init(&w, 4, 300);
    time_t now = 4000;

    /* Push more entries than capacity */
    for (int i = 1; i <= 6; i++) {
        window_push(&w, (uint64_t)(i * 10), now + i);
    }

    /* Only the 4 most recent should be retained */
    assert(window_count(&w, now + 6) == 4);

    window_stats_t s = window_aggregate(&w, now + 6);
    assert(s.min == 30); /* values 30,40,50,60 */
    assert(s.max == 60);
    printf("PASS test_capacity_wrap\n");
}

static void test_reset(void) {
    window_t w;
    window_init(&w, 8, 60);
    time_t now = 5000;

    window_push(&w, 100, now);
    window_push(&w, 200, now + 1);
    window_reset(&w);

    assert(window_count(&w, now + 1) == 0);
    window_stats_t s = window_aggregate(&w, now + 1);
    assert(s.count == 0);
    assert(s.sum == 0);
    printf("PASS test_reset\n");
}

int main(void) {
    test_init();
    test_push_and_count();
    test_aggregate_basic();
    test_expiry();
    test_capacity_wrap();
    test_reset();
    printf("All window tests passed.\n");
    return 0;
}
