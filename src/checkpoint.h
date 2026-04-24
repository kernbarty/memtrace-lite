#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <stdint.h>
#include <time.h>

#define CHECKPOINT_MAX_LABEL 64
#define CHECKPOINT_MAX_ENTRIES 32

typedef struct {
    char     label[CHECKPOINT_MAX_LABEL];
    uint64_t rss_kb;
    uint64_t vms_kb;
    time_t   timestamp;
    int      valid;
} checkpoint_entry_t;

typedef struct {
    checkpoint_entry_t entries[CHECKPOINT_MAX_ENTRIES];
    int                count;
} checkpoint_store_t;

/* Initialize a checkpoint store */
void checkpoint_init(checkpoint_store_t *store);

/* Record a named checkpoint with current memory values */
int checkpoint_record(checkpoint_store_t *store,
                      const char *label,
                      uint64_t rss_kb,
                      uint64_t vms_kb);

/* Find a checkpoint by label; returns NULL if not found */
const checkpoint_entry_t *checkpoint_find(const checkpoint_store_t *store,
                                          const char *label);

/* Compute RSS delta between two labeled checkpoints.
 * Returns 0 on success, -1 if either label is missing.
 * delta_kb is set to (end_rss - start_rss). */
int checkpoint_delta_rss(const checkpoint_store_t *store,
                         const char *start_label,
                         const char *end_label,
                         int64_t *delta_kb);

/* Remove a checkpoint by label; returns 0 on success, -1 if not found */
int checkpoint_remove(checkpoint_store_t *store, const char *label);

/* Reset all checkpoints */
void checkpoint_reset(checkpoint_store_t *store);

#endif /* CHECKPOINT_H */
