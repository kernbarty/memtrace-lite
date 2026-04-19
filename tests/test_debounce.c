#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "../src/debounce.h"

static int passed = 0;
static int failed = 0;

#define TEST(name) static void name(void)
#define RUN(name) do { printf("  " #name "... "); name(); printf("OK\n"); passed++; } while(0)

TEST(test_init) {
    Debounce db;
    debounce_init(&db, 500, 3);
    assert(!debounce_is_active(&db));
    assert(debounce_count(&db) == 0);
}

TEST(test_no_signal) {
    Debounce db;
    debounce_init(&db, 500, 2);
    bool result = debounce_update(&db, false, 1000);
    assert(!result);
    assert(!debounce_is_active(&db));
    assert(debounce_count(&db) == 0);
}

TEST(test_single_signal_not_enough) {
    Debounce db;
    debounce_init(&db, 500, 3);
    bool r = debounce_update(&db, true, 1000);
    assert(!r);
    assert(debounce_count(&db) == 1);
}

TEST(test_activates_at_min_count) {
    Debounce db;
    debounce_init(&db, 500, 3);
    debounce_update(&db, true, 1000);
    debounce_update(&db, true, 1100);
    bool r = debounce_update(&db, true, 1200);
    assert(r);
    assert(debounce_is_active(&db));
    assert(debounce_count(&db) == 3);
}

TEST(test_window_expiry_resets) {
    Debounce db;
    debounce_init(&db, 200, 3);
    debounce_update(&db, true, 1000);
    debounce_update(&db, true, 1100);
    /* gap exceeds window */
    bool r = debounce_update(&db, true, 1300);
    assert(!r);
    assert(debounce_count(&db) == 1);
}

TEST(test_reset_clears_state) {
    Debounce db;
    debounce_init(&db, 500, 2);
    debounce_update(&db, true, 1000);
    debounce_update(&db, true, 1100);
    assert(debounce_is_active(&db));
    debounce_reset(&db);
    assert(!debounce_is_active(&db));
    assert(debounce_count(&db) == 0);
}

TEST(test_false_signal_resets) {
    Debounce db;
    debounce_init(&db, 500, 2);
    debounce_update(&db, true, 1000);
    debounce_update(&db, true, 1050);
    assert(debounce_is_active(&db));
    debounce_update(&db, false, 1100);
    assert(!debounce_is_active(&db));
}

TEST(test_null_safety) {
    debounce_init(NULL, 100, 2);
    bool r = debounce_update(NULL, true, 1000);
    assert(!r);
    assert(!debounce_is_active(NULL));
    assert(debounce_count(NULL) == 0);
}

int main(void) {
    printf("test_debounce\n");
    RUN(test_init);
    RUN(test_no_signal);
    RUN(test_single_signal_not_enough);
    RUN(test_activates_at_min_count);
    RUN(test_window_expiry_resets);
    RUN(test_reset_clears_state);
    RUN(test_false_signal_resets);
    RUN(test_null_safety);
    printf("Results: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
