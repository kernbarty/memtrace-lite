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
    /* If clock goes backwards, treat elapsed as zero to avoid amplification */
    if (elapsed < 0.0) elapsed = 0.0;
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

/* Returns the number of seconds until the value decays below the given threshold.
 * Returns -1.0 if the value is already at or below the threshold, or if
 * half_life is invalid. */
double decay_time_to_threshold(const DecayState *d, double threshold) {
    if (d->half_life <= 0.0 || d->value <= threshold) return -1.0;
    /* value * 0.5^(t/half_life) = threshold => t = half_life * log2(value/threshold) */
    return d->half_life * log2(d->value / threshold);
}
