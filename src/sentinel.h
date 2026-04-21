/**
 * sentinel.h - Memory sentinel: watches a set of guards and escalates
 *              violations through a unified policy interface.
 *
 * A sentinel aggregates multiple guard checks and fence policies,
 * coordinating their evaluation and routing violations to the
 * appropriate escalation or suppression handlers.
 */

#ifndef SENTINEL_H
#define SENTINEL_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include "guard.h"
#include "fence.h"
#include "escalate.h"
#include "suppress.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SENTINEL_MAX_GUARDS  8
#define SENTINEL_MAX_FENCES  8

typedef enum {
    SENTINEL_OK        = 0,  /* all checks passed */
    SENTINEL_WARN      = 1,  /* soft limit breached */
    SENTINEL_BREACH    = 2,  /* hard limit breached */
    SENTINEL_CRITICAL  = 3   /* critical threshold exceeded */
} sentinel_status_t;

typedef struct {
    GuardCtx    *guards[SENTINEL_MAX_GUARDS];
    size_t       guard_count;

    FenceCtx    *fences[SENTINEL_MAX_FENCES];
    size_t       fence_count;

    EscalateCtx *escalator;   /* optional escalation handler */
    SuppressCtx *suppressor;  /* optional suppression handler */

    sentinel_status_t last_status;
    time_t            last_check_ts;
    uint64_t          check_count;
    uint64_t          breach_count;
} SentinelCtx;

/**
 * sentinel_init - Initialise a sentinel context.
 * @s: pointer to SentinelCtx to initialise
 */
void sentinel_init(SentinelCtx *s);

/**
 * sentinel_add_guard - Register a guard with the sentinel.
 * Returns 0 on success, -1 if the guard table is full.
 */
int sentinel_add_guard(SentinelCtx *s, GuardCtx *g);

/**
 * sentinel_add_fence - Register a fence with the sentinel.
 * Returns 0 on success, -1 if the fence table is full.
 */
int sentinel_add_fence(SentinelCtx *s, FenceCtx *f);

/**
 * sentinel_set_escalator - Attach an escalation handler.
 */
void sentinel_set_escalator(SentinelCtx *s, EscalateCtx *e);

/**
 * sentinel_set_suppressor - Attach a suppression handler.
 */
void sentinel_set_suppressor(SentinelCtx *s, SuppressCtx *sup);

/**
 * sentinel_check - Evaluate all registered guards and fences
 *                  against the supplied memory value (bytes).
 * Returns the aggregate sentinel_status_t.
 */
sentinel_status_t sentinel_check(SentinelCtx *s, size_t mem_bytes);

/**
 * sentinel_reset - Reset breach counters and status.
 */
void sentinel_reset(SentinelCtx *s);

#ifdef __cplusplus
}
#endif

#endif /* SENTINEL_H */
