#include "decay.h"
#include <math.h>
#include <string.h>

void decay_init(DecayState *d, double half_life_seconds) {
    memset(d, 0, sizeof(*d));
    d->half_life = half_life_seconds;
    d->value = 0.0;
    d->last_update = 0;
}

static double decay_factor(double half_life, double elapsed) {
    if (half_life <= 0.0) return 0.0;
    return pow(0.5, elapsed / half_life);
}

void decay_update(DecayState *d, double new_value, time_t now) {
    if (d->last_update == 0) {
        d->value = new_value;
        d->last_update = now;
        return;
    }
    double elapsed = difftime(now, d->last_update);
    double factor = decay_factor(d->half_life, elapsed);
    d->value = d->value * factor + new_value * (1.0 - factor);
    d->last_update = now;
}

double decay_get(const DecayState *d) {
    return d->value;
}

double decay_project(const DecayState *d, time_t future) {
    if (d->last_update == 0) return 0.0;
    double elapsed = difftime(future, d->last_update);
    if (elapsed < 0.0) elapsed = 0.0;
    return d->value * decay_factor(d->half_life, elapsed);
}

void decay_reset(DecayState *d) {
    d->value = 0.0;
    d->last_update = 0;
}
