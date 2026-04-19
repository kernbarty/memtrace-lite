#ifndef RATE_H
#define RATE_H

#include <stdint.h>
#include <time.h>

/* Computes memory change rate (bytes per second) between samples */

typedef struct {
    long prev_kb;
    long curr_kb;
    struct timespec prev_time;
    struct timespec curr_time;
    double rate_kb_per_sec;  /* positive = growth, negative = shrink */
    int valid;               /* 0 until at least two samples recorded */
} MemRate;

/* Initialize rate tracker */
void rate_init(MemRate *r);

/* Feed a new sample (kb = current RSS in KB) */
void rate_update(MemRate *r, long kb);

/* Get rate in KB/s; returns 0.0 if not yet valid */
double rate_get(const MemRate *r);

/* Returns 1 if rate exceeds threshold_kb_per_sec */
int rate_exceeds(const MemRate *r, double threshold_kb_per_sec);

/* Format rate into buf, e.g. "+12.3 KB/s" */
void rate_format(const MemRate *r, char *buf, int bufsz);

#endif /* RATE_H */
