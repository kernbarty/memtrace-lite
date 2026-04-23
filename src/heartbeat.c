#include "heartbeat.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void heartbeat_init(Heartbeat *hb, const HeartbeatConfig *cfg) {
    if (!hb || !cfg) return;
    memset(hb, 0, sizeof(*hb));
    hb->cfg = *cfg;
    hb->state = HB_STATE_ALIVE;
    hb->last_beat_ns = 0;
    hb->miss_count = 0;
    hb->total_beats = 0;
}

void heartbeat_reset(Heartbeat *hb) {
    if (!hb) return;
    hb->state = HB_STATE_ALIVE;
    hb->miss_count = 0;
    hb->last_beat_ns = 0;
    hb->total_beats = 0;
}

static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

void heartbeat_beat(Heartbeat *hb) {
    if (!hb) return;
    hb->last_beat_ns = now_ns();
    hb->total_beats++;
    hb->miss_count = 0;
    hb->state = HB_STATE_ALIVE;
}

HeartbeatState heartbeat_check(Heartbeat *hb) {
    if (!hb) return HB_STATE_DEAD;

    uint64_t now = now_ns();
    if (hb->last_beat_ns == 0) {
        /* Never beaten yet — treat as missed */
        hb->miss_count++;
    } else {
        uint64_t elapsed_ms = (now - hb->last_beat_ns) / 1000000ULL;
        if (elapsed_ms > hb->cfg.interval_ms) {
            hb->miss_count++;
        } else {
            hb->miss_count = 0;
        }
    }

    if (hb->miss_count == 0) {
        hb->state = HB_STATE_ALIVE;
    } else if (hb->miss_count < hb->cfg.miss_threshold) {
        hb->state = HB_STATE_DEGRADED;
    } else {
        hb->state = HB_STATE_DEAD;
        if (hb->cfg.on_dead) {
            hb->cfg.on_dead(hb, hb->cfg.user_data);
        }
    }

    return hb->state;
}

const char *heartbeat_state_str(HeartbeatState state) {
    switch (state) {
        case HB_STATE_ALIVE:    return "alive";
        case HB_STATE_DEGRADED: return "degraded";
        case HB_STATE_DEAD:     return "dead";
        default:                return "unknown";
    }
}

uint32_t heartbeat_miss_count(const Heartbeat *hb) {
    return hb ? hb->miss_count : 0;
}

uint64_t heartbeat_total_beats(const Heartbeat *hb) {
    return hb ? hb->total_beats : 0;
}
