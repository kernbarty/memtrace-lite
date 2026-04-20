#include <stdio.h>
#include <assert.h>
#include "../src/suppress.h"

static int passed = 0;
static int failed = 0;

#define TEST(name) static void test_##name(void)
#define RUN(name) do { printf("  %-40s", #name); test_##name(); printf("PASS\n"); passed++; } while(0)

TEST(timed_first_event_not_suppressed) {
    suppress_t s;
    suppress_init(&s, SUPPRESS_MODE_TIMED, 500, 0);
    bool r = suppress_record(&s, 1000);
    assert(r == false);
    assert(suppress_is_active(&s) == true);
}

TEST(timed_subsequent_suppressed) {
    suppress_t s;
    suppress_init(&s, SUPPRESS_MODE_TIMED, 500, 0);
    suppress_record(&s, 1000);
    bool r = suppress_record(&s, 1100);
    assert(r == true);
}

TEST(timed_expires) {
    suppress_t s;
    suppress_init(&s, SUPPRESS_MODE_TIMED, 500, 0);
    suppress_record(&s, 1000);
    bool r = suppress_check(&s, 1600);
    assert(r == false);
    assert(suppress_is_active(&s) == false);
}

TEST(timed_new_event_after_expiry) {
    suppress_t s;
    suppress_init(&s, SUPPRESS_MODE_TIMED, 500, 0);
    suppress_record(&s, 1000);
    suppress_check(&s, 1600);
    bool r = suppress_record(&s, 1600);
    assert(r == false);
    assert(suppress_is_active(&s) == true);
}

TEST(count_below_threshold) {
    suppress_t s;
    suppress_init(&s, SUPPRESS_MODE_COUNT, 1000, 3);
    suppress_record(&s, 0);
    suppress_record(&s, 100);
    bool r = suppress_record(&s, 200);
    assert(r == false);
    assert(suppress_get_count(&s) == 3);
}

TEST(count_exceeds_threshold) {
    suppress_t s;
    suppress_init(&s, SUPPRESS_MODE_COUNT, 1000, 3);
    suppress_record(&s, 0);
    suppress_record(&s, 100);
    suppress_record(&s, 200);
    bool r = suppress_record(&s, 300);
    assert(r == true);
    assert(suppress_is_active(&s) == true);
}

TEST(count_window_resets) {
    suppress_t s;
    suppress_init(&s, SUPPRESS_MODE_COUNT, 1000, 2);
    suppress_record(&s, 0);
    suppress_record(&s, 100);
    suppress_record(&s, 200); /* triggers suppress */
    bool r = suppress_record(&s, 1200); /* new window */
    assert(r == false);
    assert(suppress_get_count(&s) == 1);
}

TEST(reset_clears_state) {
    suppress_t s;
    suppress_init(&s, SUPPRESS_MODE_TIMED, 500, 0);
    suppress_record(&s, 1000);
    suppress_reset(&s);
    assert(suppress_is_active(&s) == false);
    assert(suppress_get_count(&s) == 0);
}

TEST(null_safety) {
    assert(suppress_check(NULL, 0) == false);
    assert(suppress_record(NULL, 0) == false);
    assert(suppress_is_active(NULL) == false);
    assert(suppress_get_count(NULL) == 0);
}

int main(void) {
    printf("test_suppress\n");
    RUN(timed_first_event_not_suppressed);
    RUN(timed_subsequent_suppressed);
    RUN(timed_expires);
    RUN(timed_new_event_after_expiry);
    RUN(count_below_threshold);
    RUN(count_exceeds_threshold);
    RUN(count_window_resets);
    RUN(reset_clears_state);
    RUN(null_safety);
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
