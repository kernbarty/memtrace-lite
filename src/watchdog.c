/*
 * watchdog.c - Watchdog monitor for memory usage violations
 *
 * Periodically checks memory snapshots against configured limits
 * and triggers callbacks when thresholds are breached.
 */

#include "watchdog.h"
#include "procmem.h"
#include "snapshot.h"
#include "threshold.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void watchdog_init(Watchdog *wd, const WatchdogConfig *cfg) {
    if (!wd || !cfg) return;
    memset(wd, 0, sizeof(*wd));
    wd->cfg = *cfg;
    wd->state = WD_STATE_IDLE;
    wd->violation_count = 0;
    wd->last_check_ts = 0;
    wd->consecutive_violations = 0;
}

void watchdog_reset(Watchdog *wd) {
    if (!wd) return;
    wd->state = WD_STATE_IDLE;
    wd->violation_count = 0;
    wd->consecutive_violations = 0;
    wd->last_check_ts = 0;
    memset(&wd->last_snapshot, 0, sizeof(wd->last_snapshot));
}

static long watchdog_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

WatchdogResult watchdog_check(Watchdog *wd, pid_t pid) {
    if (!wd) return WD_RESULT_ERROR;

    long now = watchdog_now_ms();
    if (wd->last_check_ts > 0 &&
        (now - wd->last_check_ts) < wd->cfg.interval_ms) {
        return WD_RESULT_SKIPPED;
    }
    wd->last_check_ts = now;

    MemSnapshot snap;
    if (snapshot_read(pid, &snap) != 0) {
        wd->state = WD_STATE_ERROR;
        return WD_RESULT_ERROR;
    }
    wd->last_snapshot = snap;

    int breached = 0;
    if (wd->cfg.rss_limit_kb > 0 && snap.rss_kb > wd->cfg.rss_limit_kb)
        breached = 1;
    if (wd->cfg.vms_limit_kb > 0 && snap.vms_kb > wd->cfg.vms_limit_kb)
        breached = 1;

    if (breached) {
        wd->violation_count++;
        wd->consecutive_violations++;
        wd->state = WD_STATE_VIOLATION;

        if (wd->cfg.on_violation) {
            WatchdogEvent ev;
            ev.pid = pid;
            ev.snap = snap;
            ev.violation_count = wd->violation_count;
            ev.consecutive = wd->consecutive_violations;
            wd->cfg.on_violation(&ev, wd->cfg.user_data);
        }

        if (wd->cfg.max_consecutive > 0 &&
            wd->consecutive_violations >= wd->cfg.max_consecutive) {
            wd->state = WD_STATE_CRITICAL;
            if (wd->cfg.on_critical) {
                WatchdogEvent ev;
                ev.pid = pid;
                ev.snap = snap;
                ev.violation_count = wd->violation_count;
                ev.consecutive = wd->consecutive_violations;
                wd->cfg.on_critical(&ev, wd->cfg.user_data);
            }
            return WD_RESULT_CRITICAL;
        }
        return WD_RESULT_VIOLATION;
    }

    wd->consecutive_violations = 0;
    if (wd->state == WD_STATE_VIOLATION || wd->state == WD_STATE_CRITICAL)
        wd->state = WD_STATE_RECOVERING;
    else
        wd->state = WD_STATE_OK;

    return WD_RESULT_OK;
}

WatchdogState watchdog_state(const Watchdog *wd) {
    if (!wd) return WD_STATE_ERROR;
    return wd->state;
}

const char *watchdog_state_str(WatchdogState state) {
    switch (state) {
        case WD_STATE_IDLE:       return "idle";
        case WD_STATE_OK:         return "ok";
        case WD_STATE_VIOLATION:  return "violation";
        case WD_STATE_CRITICAL:   return "critical";
        case WD_STATE_RECOVERING: return "recovering";
        case WD_STATE_ERROR:      return "error";
        default:                  return "unknown";
    }
}
