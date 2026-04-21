#ifndef CEILING_H
#define CEILING_H

#include <stddef.h>
#include <stdint.h>

/*
 * ceiling — Hard upper bound enforcement for memory usage metrics.
 *
 * A ceiling defines an absolute maximum value that a metric must not exceed.
 * When a value breaches the ceiling, a violation is recorded and the value
 * is capped. Unlike quota (which tracks budgets), ceiling is stateless and
 * purely enforces a hard limit per evaluation.
 */

typedef struct {
    size_t  limit;          /* hard ceiling value in bytes */
    size_t  last_value;     /* last value evaluated */
    int     violated;       /* non-zero if last evaluation exceeded limit */
    uint64_t violation_count; /* total number of violations since reset */
} Ceiling;

/* Initialize a ceiling with the given limit in bytes. */
void ceiling_init(Ceiling *c, size_t limit_bytes);

/*
 * Evaluate a value against the ceiling.
 * Returns the clamped value (min of value, limit).
 * Sets c->violated and increments c->violation_count if value > limit.
 */
size_t ceiling_eval(Ceiling *c, size_t value);

/* Returns non-zero if the last evaluated value violated the ceiling. */
int ceiling_is_violated(const Ceiling *c);

/* Returns the number of violations recorded since last reset. */
uint64_t ceiling_violation_count(const Ceiling *c);

/* Reset violation count and state (limit is preserved). */
void ceiling_reset(Ceiling *c);

/* Update the ceiling limit. Resets violation state. */
void ceiling_set_limit(Ceiling *c, size_t new_limit_bytes);

#endif /* CEILING_H */
