#ifndef GUARD_H
#define GUARD_H

#include <stddef.h>
#include "fence_policy.h"

/* Guard: wraps a FencePolicy and tracks consecutive breaches */

typedef struct {
    FencePolicy  policy;
    size_t       breach_count;   /* consecutive soft-limit breaches */
    size_t       hard_breach_count;
    size_t       last_value;
    int          armed;          /* 0 = disarmed, 1 = armed */
} Guard;

void        guard_init(Guard *g, FencePolicyType type,
                       size_t soft_limit, size_t hard_limit);
void        guard_arm(Guard *g);
void        guard_disarm(Guard *g);
FenceAction guard_check(Guard *g, size_t value);
void        guard_reset(Guard *g);
size_t      guard_breach_count(const Guard *g);
int         guard_is_armed(const Guard *g);

#endif /* GUARD_H */
