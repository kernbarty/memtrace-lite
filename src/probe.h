#ifndef MEMTRACE_PROBE_H
#define MEMTRACE_PROBE_H

#include <stdint.h>
#include <time.h>

/* A probe captures a named memory measurement at a specific point in time */

#define PROBE_NAME_MAX 64
#define PROBE_TAG_MAX  32

typedef enum {
    PROBE_KIND_RSS   = 0,  /* Resident Set Size in KB */
    PROBE_KIND_VSZ   = 1,  /* Virtual memory size in KB */
    PROBE_KIND_HEAP  = 2,  /* Heap usage in KB */
    PROBE_KIND_DELTA = 3   /* Delta from previous probe in KB */
} ProbeKind;

typedef struct {
    char        name[PROBE_NAME_MAX];  /* human-readable label */
    char        tag[PROBE_TAG_MAX];    /* optional grouping tag */
    ProbeKind   kind;
    uint64_t    value_kb;              /* measured value */
    uint64_t    baseline_kb;          /* baseline at probe creation */
    int64_t     delta_kb;             /* signed delta from baseline */
    struct timespec timestamp;
    uint32_t    seq;                   /* monotonic sequence number */
} Probe;

/* Initialise a probe with name/tag and kind; reads current memory immediately */
int  probe_init(Probe *p, const char *name, const char *tag, ProbeKind kind);

/* Re-sample the probe, updating value_kb, delta_kb and timestamp */
int  probe_sample(Probe *p);

/* Reset baseline to current value */
void probe_reset_baseline(Probe *p);

/* Return elapsed milliseconds since the probe was last sampled */
long probe_age_ms(const Probe *p);

/* Write a one-line human-readable summary into buf (returns buf) */
char *probe_format(const Probe *p, char *buf, size_t len);

#endif /* MEMTRACE_PROBE_H */
