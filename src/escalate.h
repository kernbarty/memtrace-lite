#ifndef ESCALATE_H
#define ESCALATE_H

#include <stdint.h>

/**
 * escalate — Tracks repeated trigger events and promotes severity level
 * based on configurable count thresholds. Supports optional auto-reset
 * after a quiet period.
 */

typedef enum {
    ESCALATE_LEVEL_NONE     = 0,
    ESCALATE_LEVEL_NOTICE   = 1,
    ESCALATE_LEVEL_WARNING  = 2,
    ESCALATE_LEVEL_CRITICAL = 3
} escalate_level_t;

typedef struct {
    uint32_t notice_threshold;   /* triggers to reach NOTICE   */
    uint32_t warning_threshold;  /* triggers to reach WARNING  */
    uint32_t critical_threshold; /* triggers to reach CRITICAL */
    uint64_t auto_reset_ms;      /* ms of silence before reset (0 = disabled) */
} escalate_config_t;

typedef struct {
    escalate_config_t cfg;
    escalate_level_t  current_level;
    uint32_t          trigger_count;
    uint64_t          last_trigger_time;
} escalate_t;

void             escalate_init(escalate_t *e, const escalate_config_t *cfg);
void             escalate_reset(escalate_t *e);
escalate_level_t escalate_update(escalate_t *e, int triggered, uint64_t now_ms);
escalate_level_t escalate_current_level(const escalate_t *e);
uint32_t         escalate_trigger_count(const escalate_t *e);
const char      *escalate_level_name(escalate_level_t level);

#endif /* ESCALATE_H */
