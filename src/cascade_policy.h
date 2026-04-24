#ifndef MEMTRACE_CASCADE_POLICY_H
#define MEMTRACE_CASCADE_POLICY_H

#include "cascade.h"

/*
 * cascade_policy — named presets for common cascade configurations.
 */

typedef enum {
    CASCADE_POLICY_CONSERVATIVE, /* gentle: log -> alert -> throttle */
    CASCADE_POLICY_AGGRESSIVE,   /* fast:  alert -> throttle -> kill  */
    CASCADE_POLICY_MONITOR_ONLY  /* never kill, only log + alert       */
} CascadePolicyPreset;

/* Apply a named preset to an already-initialised Cascade. */
void cascade_policy_apply(Cascade *c, CascadePolicyPreset preset,
                          size_t base_kb);

/* Return a human-readable name for a preset. */
const char *cascade_policy_name(CascadePolicyPreset preset);

#endif /* MEMTRACE_CASCADE_POLICY_H */
