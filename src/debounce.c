#include "debounce.h"
#include <string.h>

void debounce_init(Debounce *db, uint64_t window_ms, unsigned int min_count) {
    if (!db) return;
    memset(db, 0, sizeof(*db));
    db->window_ms = window_ms;
    db->min_count = min_count > 0 ? min_count : 1;
    db->active = false;
    db->first_seen_ms = 0;
    db->count = 0;
}

void debounce_reset(Debounce *db) {
    if (!db) return;
    db->active = false;
    db->first_seen_ms = 0;
    db->count = 0;
}

bool debounce_update(Debounce *db, bool signal, uint64_t now_ms) {
    if (!db) return false;

    if (!signal) {
        debounce_reset(db);
        return false;
    }

    if (db->count == 0) {
        db->first_seen_ms = now_ms;
        db->count = 1;
        db->active = false;
        return false;
    }

    uint64_t elapsed = now_ms - db->first_seen_ms;
    if (elapsed > db->window_ms) {
        /* window expired, restart */
        db->first_seen_ms = now_ms;
        db->count = 1;
        db->active = false;
        return false;
    }

    db->count++;

    if (!db->active && db->count >= db->min_count) {
        db->active = true;
    }

    return db->active;
}

bool debounce_is_active(const Debounce *db) {
    if (!db) return false;
    return db->active;
}

unsigned int debounce_count(const Debounce *db) {
    if (!db) return 0;
    return db->count;
}
