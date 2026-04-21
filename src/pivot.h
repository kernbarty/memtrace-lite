#ifndef PIVOT_H
#define PIVOT_H

#include <stddef.h>
#include <stdint.h>

/*
 * pivot — rolling pivot point tracker for memory usage.
 *
 * Tracks a reference (pivot) value derived from a sliding window of
 * samples. Useful for detecting sustained shifts above or below a
 * dynamically computed midpoint.
 */

typedef struct {
    double  *samples;       /* circular buffer of samples          */
    size_t   capacity;      /* max number of samples               */
    size_t   count;         /* current number of samples stored    */
    size_t   head;          /* index of next write position        */
    double   pivot;         /* current pivot value (midpoint)      */
    double   upper;         /* pivot + half-range                  */
    double   lower;         /* pivot - half-range                  */
} Pivot;

/* Initialise pivot with a pre-allocated sample window of `capacity`. */
int  pivot_init(Pivot *p, size_t capacity);

/* Push a new sample; recomputes pivot, upper, lower.               */
void pivot_update(Pivot *p, double value);

/* Returns 1 if `value` is above the upper band, -1 if below lower,
 * 0 if within band. Returns 0 when fewer than 2 samples exist.     */
int  pivot_classify(const Pivot *p, double value);

/* Reset state but keep allocated buffer.                           */
void pivot_reset(Pivot *p);

/* Free internal buffer.                                            */
void pivot_free(Pivot *p);

#endif /* PIVOT_H */
