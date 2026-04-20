#include "suppress.h"
#include <string.h>

void suppress_init(suppress_t *s, suppress_mode_t mode, uint32_t window_ms, uint32_t max_count) {
    if (!s) return;
    memset(s, 0, sizeof(*s));
    s->mode = mode;
    s->window_ms = window_ms;
    s->max_count = max_count;
    s->active = false;
    s->count = 0;
    s->window_start_ms = 0;
}

void suppress_reset(suppress_t *s) {
    if (!s) return;
    s->active = false;
    s->count = 0;
    s->window_start_ms = 0;
    s->suppress_until_ms = 0;
}

bool suppress_check(suppress_t *s, uint32_t now_ms) {
    if (!s) return false;

    if (s->mode == SUPPRESS_MODE_TIMED) {
        if (s->active && now_ms < s->suppress_until_ms) {
            return true;
        }
        if (s->active && now_ms >= s->suppress_until_ms) {
            s->active = false;
            s->count = 0;
        }
        return false;
    }

    if (s->mode == SUPPRESS_MODE_COUNT) {
        if (now_ms - s->window_start_ms > s->window_ms) {
            s->window_start_ms = now_ms;
            s->count = 0;
            s->active = false;
        }
        return s->active;
    }

    return false;
}

bool suppress_record(suppress_t *s, uint32_t now_ms) {
    if (!s) return false;

    if (s->mode == SUPPRESS_MODE_TIMED) {
        if (!s->active) {
            s->active = true;
            s->suppress_until_ms = now_ms + s->window_ms;
            s->count = 1;
            return false;
        }
        s->count++;
        return now_ms < s->suppress_until_ms;
    }

    if (s->mode == SUPPRESS_MODE_COUNT) {
        if (now_ms - s->window_start_ms > s->window_ms) {
            s->window_start_ms = now_ms;
            s->count = 0;
            s->active = false;
        }
        s->count++;
        if (s->count > s->max_count) {
            s->active = true;
            return true;
        }
        return false;
    }

    return false;
}

uint32_t suppress_get_count(const suppress_t *s) {
    return s ? s->count : 0;
}

bool suppress_is_active(const suppress_t *s) {
    return s ? s->active : false;
}
