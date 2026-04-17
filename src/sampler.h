#ifndef SAMPLER_H
#define SAMPLER_H

#include <sys/types.h>
#include "snapshot.h"

typedef struct {
    pid_t           pid;
    unsigned int    interval_ms;
    unsigned int    max_samples;   /* 0 = unlimited */
    snapshot_log_t *log;           /* caller-owned */
} sampler_config_t;

/*
 * sampler_run: blocking call that samples memory every interval_ms
 * milliseconds. Stops when max_samples reached (if non-zero) or
 * sampler_stop() is called from a signal handler.
 *
 * Returns number of samples collected, or -1 on error.
 */
int  sampler_run(const sampler_config_t *cfg);
void sampler_stop(void);   /* async-signal-safe flag setter */

#endif /* SAMPLER_H */
