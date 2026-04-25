/**
 * tally.h - Cumulative event counter with per-category tracking
 *
 * Maintains named counters for categorized memory events (allocs,
 * frees, overflows, etc.) with reset and snapshot capabilities.
 */
#ifndef MEMTRACE_TALLY_H
#define MEMTRACE_TALLY_H

#include <stdint.h>
#include <stddef.h>

#define TALLY_MAX_CATEGORIES 16
#define TALLY_NAME_LEN       32

typedef struct {
    char     name[TALLY_NAME_LEN];
    uint64_t count;
    uint64_t total;   /* sum of values passed to tally_add */
} TallyCategory;

typedef struct {
    TallyCategory categories[TALLY_MAX_CATEGORIES];
    int           num_categories;
    uint64_t      generation; /* incremented on each reset */
} Tally;

/**
 * Initialize a Tally instance.
 */
void tally_init(Tally *t);

/**
 * Register a named category. Returns category index or -1 on failure.
 */
int tally_register(Tally *t, const char *name);

/**
 * Increment count by 1 and add value to total for the given category.
 * Returns 0 on success, -1 if index is invalid.
 */
int tally_add(Tally *t, int cat_idx, uint64_t value);

/**
 * Increment count only (value contribution = 0).
 */
int tally_inc(Tally *t, int cat_idx);

/**
 * Retrieve a category by name. Returns NULL if not found.
 */
const TallyCategory *tally_get(const Tally *t, const char *name);

/**
 * Reset all counters; bumps generation.
 */
void tally_reset(Tally *t);

/**
 * Write a human-readable summary to buf (at most buf_len bytes).
 * Returns number of bytes written (excluding NUL).
 */
int tally_format(const Tally *t, char *buf, size_t buf_len);

#endif /* MEMTRACE_TALLY_H */
