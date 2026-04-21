/*
 * sentinel.c - Watchpoint-style sentinel for memory region boundary violations
 */

#include "sentinel.h"
#include <string.h>
#include <stdio.h>

void sentinel_init(Sentinel *s, const char *name, size_t warn_kb, size_t critical_kb) {
    if (!s) return;
    memset(s, 0, sizeof(*s));
    strncpy(s->name, name ? name : "unnamed", SENTINEL_NAME_MAX - 1);
    s->warn_kb = warn_kb;
    s->critical_kb = critical_kb;
    s->state = SENTINEL_OK;
    s->trip_count = 0;
    s->last_value_kb = 0;
    s->callback = NULL;
    s->user_data = NULL;
}

void sentinel_set_callback(Sentinel *s, sentinel_cb cb, void *user_data) {
    if (!s) return;
    s->callback = cb;
    s->user_data = user_data;
}

SentinelState sentinel_check(Sentinel *s, size_t current_kb) {
    if (!s) return SENTINEL_OK;

    SentinelState prev = s->state;
    s->last_value_kb = current_kb;

    if (current_kb >= s->critical_kb && s->critical_kb > 0) {
        s->state = SENTINEL_CRITICAL;
    } else if (current_kb >= s->warn_kb && s->warn_kb > 0) {
        s->state = SENTINEL_WARN;
    } else {
        s->state = SENTINEL_OK;
    }

    if (s->state > SENTINEL_OK && s->state >= prev) {
        s->trip_count++;
        if (s->callback) {
            SentinelEvent ev;
            strncpy(ev.name, s->name, SENTINEL_NAME_MAX - 1);
            ev.state = s->state;
            ev.value_kb = current_kb;
            ev.trip_count = s->trip_count;
            s->callback(&ev, s->user_data);
        }
    }

    return s->state;
}

void sentinel_reset(Sentinel *s) {
    if (!s) return;
    s->state = SENTINEL_OK;
    s->trip_count = 0;
    s->last_value_kb = 0;
}

const char *sentinel_state_str(SentinelState state) {
    switch (state) {
        case SENTINEL_OK:       return "OK";
        case SENTINEL_WARN:     return "WARN";
        case SENTINEL_CRITICAL: return "CRITICAL";
        default:                return "UNKNOWN";
    }
}

void sentinel_dump(const Sentinel *s, FILE *out) {
    if (!s || !out) return;
    fprintf(out, "Sentinel[%s] state=%s last=%zu KB warn=%zu KB critical=%zu KB trips=%u\n",
        s->name, sentinel_state_str(s->state),
        s->last_value_kb, s->warn_kb, s->critical_kb, s->trip_count);
}
