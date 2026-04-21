#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "../src/watchdog.h"
#include "../src/snapshot.h"

static int violation_fired = 0;
static int critical_fired  = 0;
static unsigned int last_consecutive = 0;

static void on_violation(const WatchdogEvent *ev, void *ud) {
    (void)ud;
    violation_fired++;
    last_consecutive = ev->consecutive;
}

static void on_critical(const WatchdogEvent *ev, void *ud) {
    (void)ud;
    critical_fired++;
    last_consecutive = ev->consecutive;
}

static void reset_counters(void) {
    violation_fired  = 0;
    critical_fired   = 0;
    last_consecutive = 0;
}

static void test_init_and_reset(void) {
    Watchdog wd;
    WatchdogConfig cfg = {0};
    cfg.interval_ms = 0;
    cfg.rss_limit_kb = 1024;

    watchdog_init(&wd, &cfg);
    assert(watchdog_state(&wd) == WD_STATE_IDLE);
    assert(wd.violation_count == 0);

    watchdog_reset(&wd);
    assert(watchdog_state(&wd) == WD_STATE_IDLE);
    printf("PASS: test_init_and_reset\n");
}

static void test_check_self_no_limit(void) {
    Watchdog wd;
    WatchdogConfig cfg = {0};
    cfg.interval_ms  = 0;
    cfg.rss_limit_kb = 0; /* disabled */
    cfg.vms_limit_kb = 0;

    watchdog_init(&wd, &cfg);
    WatchdogResult r = watchdog_check(&wd, getpid());
    /* No limits set, should be OK */
    assert(r == WD_RESULT_OK);
    assert(watchdog_state(&wd) == WD_STATE_OK);
    printf("PASS: test_check_self_no_limit\n");
}

static void test_violation_callback(void) {
    reset_counters();
    Watchdog wd;
    WatchdogConfig cfg = {0};
    cfg.interval_ms  = 0;
    cfg.rss_limit_kb = 1;  /* 1 KB — will always be exceeded */
    cfg.on_violation = on_violation;
    cfg.max_consecutive = 0;

    watchdog_init(&wd, &cfg);
    WatchdogResult r = watchdog_check(&wd, getpid());
    assert(r == WD_RESULT_VIOLATION);
    assert(violation_fired == 1);
    assert(wd.violation_count == 1);
    assert(watchdog_state(&wd) == WD_STATE_VIOLATION);
    printf("PASS: test_violation_callback\n");
}

static void test_critical_after_consecutive(void) {
    reset_counters();
    Watchdog wd;
    WatchdogConfig cfg = {0};
    cfg.interval_ms     = 0;
    cfg.rss_limit_kb    = 1; /* always exceeded */
    cfg.max_consecutive = 3;
    cfg.on_violation    = on_violation;
    cfg.on_critical     = on_critical;

    watchdog_init(&wd, &cfg);
    WatchdogResult r;
    for (int i = 0; i < 2; i++) {
        r = watchdog_check(&wd, getpid());
        assert(r == WD_RESULT_VIOLATION);
    }
    r = watchdog_check(&wd, getpid());
    assert(r == WD_RESULT_CRITICAL);
    assert(critical_fired == 1);
    assert(watchdog_state(&wd) == WD_STATE_CRITICAL);
    printf("PASS: test_critical_after_consecutive\n");
}

static void test_interval_skip(void) {
    Watchdog wd;
    WatchdogConfig cfg = {0};
    cfg.interval_ms  = 60000; /* 60 s — will never pass in test */
    cfg.rss_limit_kb = 0;

    watchdog_init(&wd, &cfg);
    /* Force first check to populate last_check_ts */
    watchdog_check(&wd, getpid());
    /* Second call should be skipped */
    WatchdogResult r = watchdog_check(&wd, getpid());
    assert(r == WD_RESULT_SKIPPED);
    printf("PASS: test_interval_skip\n");
}

static void test_state_str(void) {
    assert(strcmp(watchdog_state_str(WD_STATE_IDLE),       "idle")       == 0);
    assert(strcmp(watchdog_state_str(WD_STATE_OK),         "ok")         == 0);
    assert(strcmp(watchdog_state_str(WD_STATE_VIOLATION),  "violation")  == 0);
    assert(strcmp(watchdog_state_str(WD_STATE_CRITICAL),   "critical")   == 0);
    assert(strcmp(watchdog_state_str(WD_STATE_RECOVERING), "recovering") == 0);
    assert(strcmp(watchdog_state_str(WD_STATE_ERROR),      "error")      == 0);
    printf("PASS: test_state_str\n");
}

int main(void) {
    test_init_and_reset();
    test_check_self_no_limit();
    test_violation_callback();
    test_critical_after_consecutive();
    test_interval_skip();
    test_state_str();
    printf("All watchdog tests passed.\n");
    return 0;
}
