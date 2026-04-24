#include "latch.h"
#include <string.h>

void latch_init(latch_t *l, const char *label)
{
    memset(l, 0, sizeof(*l));
    l->state = LATCH_ARMED;
    l->label = label;
}

void latch_arm(latch_t *l)
{
    if (l->state == LATCH_DISARMED || l->state == LATCH_TRIGGERED) {
        l->state         = LATCH_ARMED;
        l->trigger_value = 0;
        l->trigger_time  = 0;
        /* preserve trigger_count across re-arms */
    }
}

bool latch_trigger(latch_t *l, uint64_t value)
{
    if (l->state != LATCH_ARMED)
        return false;

    l->state         = LATCH_TRIGGERED;
    l->trigger_value = value;
    l->trigger_time  = time(NULL);
    l->trigger_count++;
    return true;
}

bool latch_is_triggered(const latch_t *l)
{
    return l->state == LATCH_TRIGGERED;
}

void latch_reset(latch_t *l)
{
    latch_arm(l);
}

void latch_disarm(latch_t *l)
{
    l->state = LATCH_DISARMED;
}
