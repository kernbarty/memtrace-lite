#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "../src/heartbeat.h"

static int dead_called = 0;

static void on_dead_cb(Heartbeat *hb, void *user_data) {
    (void)hb;
    int *flag = (int *)user_data;
    *flag = 1;
}

static void test_initial_state(void) {
    Heartbeat hb;
    HeartbeatConfig cfg = {
        .interval_ms    = 100,
        .miss_threshold = 3,
        .on_dead        = NULL,
        .user_data      = NULL
    };
    heartbeat_init(&hb, &cfg);
    assert(hb.state == HB_STATE_ALIVE);
    assert(heartbeat_miss_count(&hb) == 0);
    assert(heartbeat_total_beats(&hb) == 0);
    printf("PASS: test_initial_state\n");
}

static void test_beat_increments_total(void) {
    Heartbeat hb;
    HeartbeatConfig cfg = { .interval_ms = 500, .miss_threshold = 3 };
    heartbeat_init(&hb, &cfg);
    heartbeat_beat(&hb);
    heartbeat_beat(&hb);
    assert(heartbeat_total_beats(&hb) == 2);
    printf("PASS: test_beat_increments_total\n");
}

static void test_alive_after_timely_beat(void) {
    Heartbeat hb;
    HeartbeatConfig cfg = { .interval_ms = 500, .miss_threshold = 3 };
    heartbeat_init(&hb, &cfg);
    heartbeat_beat(&hb);
    HeartbeatState s = heartbeat_check(&hb);
    assert(s == HB_STATE_ALIVE);
    printf("PASS: test_alive_after_timely_beat\n");
}

static void test_degraded_on_missed_beats(void) {
    Heartbeat hb;
    HeartbeatConfig cfg = { .interval_ms = 1, .miss_threshold = 5 };
    heartbeat_init(&hb, &cfg);
    /* Never beat — each check increments miss_count */
    heartbeat_check(&hb); /* miss=1 */
    heartbeat_check(&hb); /* miss=2 */
    HeartbeatState s = heartbeat_check(&hb); /* miss=3 */
    assert(s == HB_STATE_DEGRADED);
    assert(heartbeat_miss_count(&hb) == 3);
    printf("PASS: test_degraded_on_missed_beats\n");
}

static void test_dead_callback_fires(void) {
    dead_called = 0;
    Heartbeat hb;
    HeartbeatConfig cfg = {
        .interval_ms    = 1,
        .miss_threshold = 2,
        .on_dead        = on_dead_cb,
        .user_data      = &dead_called
    };
    heartbeat_init(&hb, &cfg);
    heartbeat_check(&hb); /* miss=1 -> degraded */
    heartbeat_check(&hb); /* miss=2 -> dead, callback fires */
    assert(hb.state == HB_STATE_DEAD);
    assert(dead_called == 1);
    printf("PASS: test_dead_callback_fires\n");
}

static void test_reset_clears_state(void) {
    Heartbeat hb;
    HeartbeatConfig cfg = { .interval_ms = 1, .miss_threshold = 2 };
    heartbeat_init(&hb, &cfg);
    heartbeat_check(&hb);
    heartbeat_check(&hb);
    assert(hb.state == HB_STATE_DEAD);
    heartbeat_reset(&hb);
    assert(hb.state == HB_STATE_ALIVE);
    assert(heartbeat_miss_count(&hb) == 0);
    assert(heartbeat_total_beats(&hb) == 0);
    printf("PASS: test_reset_clears_state\n");
}

static void test_state_str(void) {
    assert(heartbeat_state_str(HB_STATE_ALIVE)    != NULL);
    assert(heartbeat_state_str(HB_STATE_DEGRADED) != NULL);
    assert(heartbeat_state_str(HB_STATE_DEAD)     != NULL);
    printf("PASS: test_state_str\n");
}

int main(void) {
    test_initial_state();
    test_beat_increments_total();
    test_alive_after_timely_beat();
    test_degraded_on_missed_beats();
    test_dead_callback_fires();
    test_reset_clears_state();
    test_state_str();
    printf("All heartbeat tests passed.\n");
    return 0;
}
