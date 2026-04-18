#include "ringbuf.h"
#include <stdlib.h>
#include <string.h>

RingBuf *ringbuf_create(size_t capacity) {
    if (capacity == 0) return NULL;
    RingBuf *rb = malloc(sizeof(RingBuf));
    if (!rb) return NULL;
    rb->data = malloc(sizeof(MemSnapshot) * capacity);
    if (!rb->data) { free(rb); return NULL; }
    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    return rb;
}

void ringbuf_destroy(RingBuf *rb) {
    if (!rb) return;
    free(rb->data);
    free(rb);
}

bool ringbuf_push(RingBuf *rb, const MemSnapshot *snap) {
    if (!rb || !snap) return false;
    rb->data[rb->head] = *snap;
    rb->head = (rb->head + 1) % rb->capacity;
    if (rb->count < rb->capacity) {
        rb->count++;
    } else {
        rb->tail = (rb->tail + 1) % rb->capacity;
    }
    return true;
}

bool ringbuf_pop(RingBuf *rb, MemSnapshot *out) {
    if (!rb || !out || rb->count == 0) return false;
    *out = rb->data[rb->tail];
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->count--;
    return true;
}

bool ringbuf_peek(const RingBuf *rb, size_t index, MemSnapshot *out) {
    if (!rb || !out || index >= rb->count) return false;
    size_t i = (rb->tail + index) % rb->capacity;
    *out = rb->data[i];
    return true;
}

void ringbuf_clear(RingBuf *rb) {
    if (!rb) return;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

bool ringbuf_is_full(const RingBuf *rb) {
    return rb && rb->count == rb->capacity;
}

bool ringbuf_is_empty(const RingBuf *rb) {
    return !rb || rb->count == 0;
}
