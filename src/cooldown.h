#ifndef COOLDOWN_H
#define COOLDOWN_H

#include <time.h>
#include <stdbool.h>

/* Cooldown: suppresses repeated actions until a delay has elapsed */

typedef struct {
    double interval_sec;   /* minimum seconds between allowed triggers */
    struct timespec last;  /* time of last allowed trigger */
    bool armed;            /* whether a trigger has occurred at least once */
} Cooldown;

/* Initialize cooldown with given interval in seconds */
void cooldown_init(Cooldown *cd, double interval_sec);

/* Reset cooldown state (next check will always pass) */
void cooldown_reset(Cooldown *cd);

/* Returns true if the cooldown period has elapsed (action is allowed).
 * If allowed, records current time as last trigger. */
bool cooldown_check(Cooldown *cd);

/* Returns seconds remaining until cooldown expires, or 0.0 if ready */
double cooldown_remaining(const Cooldown *cd);

#endif /* COOLDOWN_H */
