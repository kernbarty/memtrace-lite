#include "history.h"
#include <stdlib.h>
#include <string.h>

bool history_init(History *h, size_t capacity) {
    if (!h || capacity == 0) return false;
    h->entries = calloc(capacity, sizeof(MemSnapshot));
    if (!h->entries) return false;
    h->capacity = capacity;
    h->count = 0;
    h->head = 0;
    return true;
}

void history_free(History *h) {
    if (!h) return;
    free(h->entries);
    h->entries = NULL;
    h->capacity = 0;
    h->count = 0;
    h->head = 0;
}

bool history_push(History *h, const MemSnapshot *snap) {
    if (!h || !snap) return false;
    h->entries[h->head] = *snap;
    h->head = (h->head + 1) % h->capacity;
    if (h->count < h->capacity) h->count++;
    return true;
}

bool history_get(const History *h, size_t index, MemSnapshot *out) {
    if (!h || !out || index >= h->count) return false;
    size_t start = (h->head + h->capacity - h->count) % h->capacity;
    size_t real = (start + index) % h->capacity;
    *out = h->entries[real];
    return true;
}

bool history_latest(const History *h, MemSnapshot *out) {
    if (!h || !out || h->count == 0) return false;
    size_t idx = (h->head + h->capacity - 1) % h->capacity;
    *out = h->entries[idx];
    return true;
}

void history_clear(History *h) {
    if (!h) return;
    h->count = 0;
    h->head = 0;
}

size_t history_count(const History *h) {
    return h ? h->count : 0;
}
