/*
 * test_sentinel.c - Unit tests for sentinel module
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/sentinel.h"

static int cb_called = 0;
static SentinelState last_cb_state = SENTINEL_OK;
static size_t last_cb_value = 0;

static void test_callback(const SentinelEvent *ev, void *user_data) {
    (void)user_data;
    cb_called++;
    last_cb_state = ev->state;
    last_cb_value = ev->value_kb;
}

static void test_init(void) {
    Sentinel s;
    sentinel_init(&s, "test", 100, 200);
    assert(s.state == SENTINEL_OK);
    assert(s.trip_count == 0);
    assert(s.warn_kb == 100);
    assert(s.critical_kb == 200);
    assert(strcmp(s.name, "test") == 0);
    printf("PASS test_init\n");
}

static void test_ok_state(void) {
    Sentinel s;
    sentinel_init(&s, "mem", 100, 200);
    SentinelState st = sentinel_check(&s, 50);
    assert(st == SENTINEL_OK);
    assert(s.trip_count == 0);
    printf("PASS test_ok_state\n");
}

static void test_warn_state(void) {
    Sentinel s;
    sentinel_init(&s, "mem", 100, 200);
    sentinel_set_callback(&s, test_callback, NULL);
    cb_called = 0;
    SentinelState st = sentinel_check(&s, 150);
    assert(st == SENTINEL_WARN);
    assert(s.trip_count == 1);
    assert(cb_called == 1);
    assert(last_cb_state == SENTINEL_WARN);
    assert(last_cb_value == 150);
    printf("PASS test_warn_state\n");
}

static void test_critical_state(void) {
    Sentinel s;
    sentinel_init(&s, "mem", 100, 200);
    sentinel_set_callback(&s, test_callback, NULL);
    cb_called = 0;
    SentinelState st = sentinel_check(&s, 250);
    assert(st == SENTINEL_CRITICAL);
    assert(s.trip_count == 1);
    assert(cb_called == 1);
    assert(last_cb_state == SENTINEL_CRITICAL);
    printf("PASS test_critical_state\n");
}

static void test_reset(void) {
    Sentinel s;
    sentinel_init(&s, "mem", 100, 200);
    sentinel_check(&s, 150);
    sentinel_reset(&s);
    assert(s.state == SENTINEL_OK);
    assert(s.trip_count == 0);
    assert(s.last_value_kb == 0);
    printf("PASS test_reset\n");
}

static void test_no_double_trip_on_sustained(void) {
    Sentinel s;
    sentinel_init(&s, "mem", 100, 200);
    sentinel_set_callback(&s, test_callback, NULL);
    cb_called = 0;
    sentinel_check(&s, 150);
    /* sustained warn — no new trip since state didn't escalate */
    sentinel_check(&s, 160);
    assert(cb_called == 1);
    printf("PASS test_no_double_trip_on_sustained\n");
}

static void test_state_str(void) {
    assert(strcmp(sentinel_state_str(SENTINEL_OK), "OK") == 0);
    assert(strcmp(sentinel_state_str(SENTINEL_WARN), "WARN") == 0);
    assert(strcmp(sentinel_state_str(SENTINEL_CRITICAL), "CRITICAL") == 0);
    printf("PASS test_state_str\n");
}

int main(void) {
    test_init();
    test_ok_state();
    test_warn_state();
    test_critical_state();
    test_reset();
    test_no_double_trip_on_sustained();
    test_state_str();
    printf("All sentinel tests passed.\n");
    return 0;
}
