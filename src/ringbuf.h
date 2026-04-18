#ifndef RINGBUF_H
#define RINGBUF_H

#include <stddef.h>
#include "snapshot.h"

/* Fixed-capacity ring buffer for storing memory snapshots.
 * Overwrites oldest entries when full. Thread-unsafe by design
 * (caller must synchronize if needed). */

#define RINGBUF_MAX_CAPACITY 1024

typedef struct {
    MemSnapshot *entries;   /* heap-allocated array of snapshots */
    size_t       capacity;  /* max number of entries */
    size_t       head;      /* index of next write position */
    size_t       count;     /* number of valid entries (<= capacity) */
} RingBuf;

/* Allocate and initialize a ring buffer with given capacity.
 * Returns NULL on failure. Capacity clamped to RINGBUF_MAX_CAPACITY. */
RingBuf *ringbuf_create(size_t capacity);

/* Free ring buffer and its internal storage. */
void ringbuf_destroy(RingBuf *rb);

/* Push a snapshot into the buffer. Overwrites oldest if full.
 * Returns 0 on success, -1 on invalid args. */
int ringbuf_push(RingBuf *rb, const MemSnapshot *snap);

/* Read entry at logical index i (0 = oldest). Returns NULL if out of range. */
const MemSnapshot *ringbuf_get(const RingBuf *rb, size_t i);

/* Return number of valid entries currently stored. */
size_t ringbuf_count(const RingBuf *rb);

/* Return the most recently pushed snapshot, or NULL if the buffer is empty. */
const MemSnapshot *ringbuf_peek_latest(const RingBuf *rb);

/* Clear all entries without freeing memory. */
void ringbuf_clear(RingBuf *rb);

#endif /* RINGBUF_H */
