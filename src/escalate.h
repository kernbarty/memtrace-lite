/*
 * escalate.h - Alert escalation logic for memtrace-lite
 *
 * Tracks repeated threshold violations and escalates severity
 * when a condition persists beyond a configurable strike count.
 */

#ifndef ESCALATE_H
#define ESCALATE_H

#include <stdint.h>

typedef enum {
    ESCALATE_LEVEL_NONE = 0,
    ESCALATE_LEVEL_WARN,
    ESCALATE_LEVEL_ERROR,
    ESCALATE_LEVEL_CRITICAL
} escalate_level_t;

typedef struct {
    int     warn_strikes;     /* violations before WARN */
    int     error_strikes;    /* violations before ERROR */
    int     critical_strikes; /* violations before CRITICAL */
    int     reset_strikes;    /* consecutive clears before reset */
} escalate_config_t;

typedef struct {
    escalate_config_t config;
    int               strike_count;   /* current violation streak */
    int               clear_count;    /* current clear streak */
    escalate_level_t  level;          /* current escalation level */
    uint64_t          last_strike_ts; /* timestamp of last violation */
} escalate_t;

void             escalate_init(escalate_t *e, const escalate_config_t *cfg);
void             escalate_reset(escalate_t *e);
escalate_level_t escalate_record_violation(escalate_t *e, uint64_t ts_ms);
escalate_level_t escalate_record_clear(escalate_t *e, uint64_t ts_ms);
escalate_level_t escalate_current_level(const escalate_t *e);
const char      *escalate_level_str(escalate_level_t level);

#endif /* ESCALATE_H */
