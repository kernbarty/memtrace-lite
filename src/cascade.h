#ifndef MEMTRACE_CASCADE_H
#define MEMTRACE_CASCADE_H

#include <stddef.h>
#include <stdbool.h>

/*
 * cascade — chain of escalating actions triggered when memory thresholds
 * are breached in sequence. Each level fires once the previous level's
 * condition has been sustained for a minimum number of ticks.
 */

#define CASCADE_MAX_LEVELS 8

typedef enum {
    CASCADE_ACTION_NONE    = 0,
    CASCADE_ACTION_LOG     = 1,
    CASCADE_ACTION_ALERT   = 2,
    CASCADE_ACTION_THROTTLE = 3,
    CASCADE_ACTION_KILL    = 4
} CascadeAction;

typedef struct {
    size_t        threshold_kb;   /* RSS threshold to trigger this level */
    unsigned int  sustain_ticks;  /* ticks threshold must hold before firing */
    CascadeAction action;         /* action to perform at this level */
    const char   *label;          /* human-readable label */
} CascadeLevel;

typedef struct {
    CascadeLevel  levels[CASCADE_MAX_LEVELS];
    int           num_levels;
    int           current_level;  /* highest level currently active (-1 = none) */
    unsigned int  ticks_at_level; /* how many ticks we've been at current_level */
    bool          fired;          /* true if current level's action was fired */
} Cascade;

void cascade_init(Cascade *c);
void cascade_add_level(Cascade *c, size_t threshold_kb, unsigned int sustain_ticks,
                       CascadeAction action, const char *label);
CascadeAction cascade_update(Cascade *c, size_t rss_kb);
void cascade_reset(Cascade *c);
int  cascade_current_level(const Cascade *c);
const char *cascade_action_name(CascadeAction action);

#endif /* MEMTRACE_CASCADE_H */
