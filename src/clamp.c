#include "clamp.h"
#include <string.h>

void clamp_init(clamp_t *c, uint64_t min, uint64_t max)
{
    if (!c)
        return;
    memset(c, 0, sizeof(*c));
    c->min = min;
    c->max = max;
}

uint64_t clamp_apply(clamp_t *c, uint64_t value)
{
    uint64_t result;

    if (!c)
        return value;

    c->total++;

    if (value < c->min) {
        result = c->min;
        c->clamped++;
    } else if (value > c->max) {
        result = c->max;
        c->clamped++;
    } else {
        result = value;
    }

    return result;
}

double clamp_ratio(const clamp_t *c)
{
    if (!c || c->total == 0)
        return 0.0;
    return (double)c->clamped / (double)c->total;
}

void clamp_reset(clamp_t *c)
{
    if (!c)
        return;
    c->clamped = 0;
    c->total   = 0;
}

void clamp_set_bounds(clamp_t *c, uint64_t min, uint64_t max)
{
    if (!c)
        return;
    if (min > max) {
        /* swap silently to maintain invariant */
        uint64_t tmp = min;
        min = max;
        max = tmp;
    }
    c->min = min;
    c->max = max;
}
