#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "../src/heartbeat.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define CHECK(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; } \
    else { fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg); } \
} while (0)

static void test_init(void) {
    Heartbeat hb;
    heartbeat_init(&hb, 1234, 5);
    CHECK(hb.pid == 1234,          "pid set correctly");
    CHECK(hb.interval_sec == 5,    "interval set correctly");
    CHECK(hb.missed == 0,          "missed starts at zero");
    CHECK(hb.status == HEARTBEAT_ALIVE, "initial status is ALIVE");
}

static void test_pulse_resets_missed(void) {
    Heartbeat hb;
    heartbeat_init(&hb, 42, 2);
    hb.missed = 3;
    hb.status = HEARTBEAT_STALE;
    heartbeat_pulse(&hb);
    CHECK(hb.missed == 0,               "pulse resets missed counter");
    CHECK(hb.status == HEARTBEAT_ALIVE, "pulse restores ALIVE status");
    CHECK(hb.last_beat > 0,             "last_beat updated");
}

static void test_check_alive_within_interval(void) {
    Heartbeat hb;
    heartbeat_init(&hb, 99, 10);
    heartbeat_pulse(&hb);
    HeartbeatStatus s = heartbeat_check(&hb);
    CHECK(s == HEARTBEAT_ALIVE, "fresh pulse => ALIVE");
    CHECK(hb.missed == 0,       "no missed beats within interval");
}

static void test_check_stale_after_deadline(void) {
    Heartbeat hb;
    heartbeat_init(&hb, 7, 1);
    /* Backdate last_beat so the interval has elapsed */
    hb.last_beat = time(NULL) - 5;
    HeartbeatStatus s = heartbeat_check(&hb);
    CHECK(s == HEARTBEAT_STALE || s == HEARTBEAT_DEAD,
          "overdue beat => STALE or DEAD");
    CHECK(hb.missed >= 1, "missed counter incremented");
}

static void test_dead_after_max_missed(void) {
    Heartbeat hb;
    heartbeat_init(&hb, 5, 1);
    hb.last_beat = time(NULL) - 100;
    /* Drive missed counter to maximum */
    for (int i = 0; i < HEARTBEAT_MAX_MISSED + 2; i++) {
        heartbeat_check(&hb);
        /* backdate again so each check sees an overdue beat */
        hb.last_beat = time(NULL) - 100;
    }
    CHECK(heartbeat_is_dead(&hb),       "is_dead returns true");
    CHECK(hb.status == HEARTBEAT_DEAD,  "status is DEAD");
}

static void test_reset(void) {
    Heartbeat hb;
    heartbeat_init(&hb, 3, 2);
    hb.missed = 4;
    hb.status = HEARTBEAT_DEAD;
    heartbeat_reset(&hb);
    CHECK(hb.missed == 0,               "reset clears missed");
    CHECK(hb.status == HEARTBEAT_ALIVE, "reset restores ALIVE");
}

static void test_status_str(void) {
    CHECK(strcmp(heartbeat_status_str(HEARTBEAT_ALIVE), "alive") == 0, "alive str");
    CHECK(strcmp(heartbeat_status_str(HEARTBEAT_STALE), "stale") == 0, "stale str");
    CHECK(strcmp(heartbeat_status_str(HEARTBEAT_DEAD),  "dead")  == 0, "dead str");
}

int main(void) {
    test_init();
    test_pulse_resets_missed();
    test_check_alive_within_interval();
    test_check_stale_after_deadline();
    test_dead_after_max_missed();
    test_reset();
    test_status_str();

    printf("heartbeat: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? EXIT_SUCCESS : EXIT_FAILURE;
}
