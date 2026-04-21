#ifndef FENCE_POLICY_H
#define FENCE_POLICY_H

#include "fence.h"

/*
 * fence_policy — action callbacks triggered by fence state transitions.
 *
 * Attach callbacks to a fence so that callers are notified automatically
 * when the soft or hard boundary is crossed.
 */

typedef void (*fence_cb_t)(fence_status_t status, size_t current_kb,
                            void *userdata);

typedef struct {
    fence_t    *fence;       /* underlying fence (not owned) */
    fence_cb_t  on_warn;     /* called when FENCE_WARN is returned */
    fence_cb_t  on_breach;   /* called when FENCE_BREACH is returned */
    void       *userdata;    /* passed verbatim to callbacks */
} fence_policy_t;

/* Bind callbacks to a fence. Passing NULL for a callback disables it. */
void fence_policy_init(fence_policy_t *p, fence_t *f,
                       fence_cb_t on_warn, fence_cb_t on_breach,
                       void *userdata);

/* Evaluate usage and fire the appropriate callback if a boundary is crossed.
 * Returns the fence status. */
fence_status_t fence_policy_check(fence_policy_t *p, size_t current_kb);

#endif /* FENCE_POLICY_H */
