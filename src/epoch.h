#ifndef EPOCH_H
#define EPOCH_H

#include <stdint.h>
#include <time.h>

/*
 * epoch — time-bounded sampling epoch tracker
 *
 * Tracks discrete sampling epochs with start/end timestamps,
 * sequence numbers, and elapsed duration helpers.
 */

typedef struct {
    uint64_t    seq;          /* monotonically increasing epoch number */
    struct timespec start;    /* wall-clock start of this epoch */
    struct timespec end;      /* wall-clock end (zero if still open) */
    uint64_t    sample_count; /* samples collected in this epoch */
    uint64_t    bytes_total;  /* total bytes observed this epoch */
} Epoch;

/* Initialise and open a new epoch (seq starts at 1) */
void epoch_open(Epoch *e, uint64_t seq);

/* Close the current epoch, recording end timestamp */
void epoch_close(Epoch *e);

/* Return elapsed milliseconds; uses current time if epoch is still open */
uint64_t epoch_elapsed_ms(const Epoch *e);

/* Return non-zero if the epoch is still open */
int epoch_is_open(const Epoch *e);

/* Accumulate a sample into the epoch */
void epoch_record(Epoch *e, uint64_t bytes);

/* Reset epoch to zero state */
void epoch_reset(Epoch *e);

#endif /* EPOCH_H */
