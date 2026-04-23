#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <stdint.h>

typedef enum {
    HB_STATE_ALIVE = 0,
    HB_STATE_DEGRADED,
    HB_STATE_DEAD
} HeartbeatState;

typedef struct Heartbeat Heartbeat;

typedef void (*heartbeat_dead_fn)(Heartbeat *hb, void *user_data);

typedef struct {
    uint64_t         interval_ms;    /* Expected beat interval in milliseconds */
    uint32_t         miss_threshold; /* Misses before transitioning to DEAD */
    heartbeat_dead_fn on_dead;       /* Optional callback when state -> DEAD */
    void            *user_data;
} HeartbeatConfig;

struct Heartbeat {
    HeartbeatConfig cfg;
    HeartbeatState  state;
    uint64_t        last_beat_ns;
    uint32_t        miss_count;
    uint64_t        total_beats;
};

void            heartbeat_init(Heartbeat *hb, const HeartbeatConfig *cfg);
void            heartbeat_reset(Heartbeat *hb);
void            heartbeat_beat(Heartbeat *hb);
HeartbeatState  heartbeat_check(Heartbeat *hb);
const char     *heartbeat_state_str(HeartbeatState state);
uint32_t        heartbeat_miss_count(const Heartbeat *hb);
uint64_t        heartbeat_total_beats(const Heartbeat *hb);

#endif /* HEARTBEAT_H */
