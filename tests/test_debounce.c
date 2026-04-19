#include <stdio.h>
#include <assert.h>
#include "../src/debounce.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond, msg) \
    do { if (cond) { printf("PASS: %s\n", msg); passed++; } \
         else { printf("FAIL: %s\n", msg); failed++; } } while(0)

static void test_first_trigger_accepted(void) {
    Debounce db;
    debounce_init(&db, 500);
    CHECK(debounce_trigger(&db, 1000), "first trigger accepted");
}

static void test_trigger_within_window_suppressed(void) {
    Debounce db;
    debounce_init(&db, 500);
    debounce_trigger(&db, 1000);
    CHECK(!debounce_trigger(&db, 1200), "trigger within window suppressed");
}

static void test_trigger_after_window_accepted(void) {
    Debounce db;
    debounce_init(&db, 500);
    debounce_trigger(&db, 1000);
    CHECK(debounce_trigger(&db, 1600), "trigger after window accepted");
}

static void test_trigger_at_exact_boundary(void) {
    Debounce db;
    debounce_init(&db, 500);
    debounce_trigger(&db, 1000);
    CHECK(debounce_trigger(&db, 1500), "trigger at exact boundary accepted");
}

static void test_reset_allows_immediate_trigger(void) {
    Debounce db;
    debounce_init(&db, 500);
    debounce_trigger(&db, 1000);
    debounce_reset(&db);
    CHECK(debounce_trigger(&db, 1100), "trigger accepted after reset");
}

static void test_remaining_within_window(void) {
    Debounce db;
    debounce_init(&db, 500);
    debounce_trigger(&db, 1000);
    uint64_t rem = debounce_remaining(&db, 1200);
    CHECK(rem == 300, "remaining ms correct within window");
}

static void test_remaining_after_window(void) {
    Debounce db;
    debounce_init(&db, 500);
    debounce_trigger(&db, 1000);
    uint64_t rem = debounce_remaining(&db, 1600);
    CHECK(rem == 0, "remaining is 0 after window expires");
}

static void test_unarmed_remaining_is_zero(void) {
    Debounce db;
    debounce_init(&db, 500);
    CHECK(debounce_remaining(&db, 0) == 0, "unarmed remaining is 0");
}

int main(void) {
    test_first_trigger_accepted();
    test_trigger_within_window_suppressed();
    test_trigger_after_window_accepted();
    test_trigger_at_exact_boundary();
    test_reset_allows_immediate_trigger();
    test_remaining_within_window();
    test_remaining_after_window();
    test_unarmed_remaining_is_zero();

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
