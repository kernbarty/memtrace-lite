#include "pivot.h"

#include <stdlib.h>
#include <string.h>

int pivot_init(Pivot *p, size_t capacity)
{
    if (!p || capacity < 2)
        return -1;

    p->samples  = calloc(capacity, sizeof(double));
    if (!p->samples)
        return -1;

    p->capacity = capacity;
    p->count    = 0;
    p->head     = 0;
    p->pivot    = 0.0;
    p->upper    = 0.0;
    p->lower    = 0.0;
    return 0;
}

void pivot_update(Pivot *p, double value)
{
    if (!p || !p->samples)
        return;

    p->samples[p->head] = value;
    p->head = (p->head + 1) % p->capacity;
    if (p->count < p->capacity)
        p->count++;

    /* Compute min and max over stored samples */
    double mn = p->samples[0];
    double mx = p->samples[0];
    size_t n  = p->count;

    for (size_t i = 1; i < n; i++) {
        double s = p->samples[i];
        if (s < mn) mn = s;
        if (s > mx) mx = s;
    }

    p->pivot = (mn + mx) / 2.0;
    double half = (mx - mn) / 2.0;
    p->upper = p->pivot + half;
    p->lower = p->pivot - half;
}

int pivot_classify(const Pivot *p, double value)
{
    if (!p || p->count < 2)
        return 0;

    if (value > p->upper)
        return  1;
    if (value < p->lower)
        return -1;
    return 0;
}

void pivot_reset(Pivot *p)
{
    if (!p || !p->samples)
        return;
    memset(p->samples, 0, p->capacity * sizeof(double));
    p->count = 0;
    p->head  = 0;
    p->pivot = 0.0;
    p->upper = 0.0;
    p->lower = 0.0;
}

void pivot_free(Pivot *p)
{
    if (!p)
        return;
    free(p->samples);
    p->samples  = NULL;
    p->capacity = 0;
    p->count    = 0;
}
