#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "../src/suppress.h"

static void test_init(void) {
    suppress_t s;
    suppress_init(&s, 10);
    assert(s.count == 0);
    assert(s.window_sec == 10);
    printf("  [PASS] test_init\n");
}

static void test_first_event_not_suppressed(void) {
    suppress_t s;
    suppress_init(&s, 10);
    bool suppressed = suppress_check(&s, "oom_alert");
    assert(!suppressed);
    assert(s.count == 1);
    printf("  [PASS] test_first_event_not_suppressed\n");
}

static void test_repeat_within_window_suppressed(void) {
    suppress_t s;
    suppress_init(&s, 60);
    suppress_check(&s, "leak_alert");
    bool suppressed = suppress_check(&s, "leak_alert");
    assert(suppressed);
    assert(suppress_count(&s, "leak_alert") >= 1);
    printf("  [PASS] test_repeat_within_window_suppressed\n");
}

static void test_different_keys_independent(void) {
    suppress_t s;
    suppress_init(&s, 60);
    suppress_check(&s, "key_a");
    suppress_check(&s, "key_a");
    bool b = suppress_check(&s, "key_b");
    assert(!b);
    printf("  [PASS] test_different_keys_independent\n");
}

static void test_reset_clears_suppression(void) {
    suppress_t s;
    suppress_init(&s, 60);
    suppress_check(&s, "rss_spike");
    suppress_check(&s, "rss_spike"); /* now suppressed */
    suppress_reset(&s, "rss_spike");
    bool suppressed = suppress_check(&s, "rss_spike");
    assert(!suppressed);
    printf("  [PASS] test_reset_clears_suppression\n");
}

static void test_expire_removes_old_entries(void) {
    suppress_t s;
    suppress_init(&s, 1); /* 1 second window */
    suppress_check(&s, "short_lived");
    assert(s.count == 1);
    sleep(2);
    suppress_expire(&s);
    assert(suppress_count(&s, "short_lived") == 0);
    printf("  [PASS] test_expire_removes_old_entries\n");
}

static void test_reset_all(void) {
    suppress_t s;
    suppress_init(&s, 60);
    suppress_check(&s, "a");
    suppress_check(&s, "b");
    suppress_reset_all(&s);
    assert(s.count == 0);
    printf("  [PASS] test_reset_all\n");
}

int main(void) {
    printf("Running suppress tests...\n");
    test_init();
    test_first_event_not_suppressed();
    test_repeat_within_window_suppressed();
    test_different_keys_independent();
    test_reset_clears_suppression();
    test_expire_removes_old_entries();
    test_reset_all();
    printf("All suppress tests passed.\n");
    return 0;
}
