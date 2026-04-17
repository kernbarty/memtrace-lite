#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdatomic.h>
#include "sampler.h"

static volatile atomic_int g_stop_flag = 0;

void sampler_stop(void) {
    atomic_store(&g_stop_flag, 1);
}

static void sleep_ms(unsigned int ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

int sampler_run(const sampler_config_t *cfg) {
    if (!cfg || !cfg->log || cfg->interval_ms == 0) return -1;

    atomic_store(&g_stop_flag, 0);

    int collected = 0;

    while (!atomic_load(&g_stop_flag)) {
        mem_snapshot_t snap;
        if (snapshot_take(cfg->pid, &snap) == 0) {
            if (snapshot_log_append(cfg->log, &snap) != 0) {
                fprintf(stderr, "sampler: failed to append snapshot\n");
                return -1;
            }
            collected++;
        } else {
            fprintf(stderr, "sampler: failed to read pid %d\n", cfg->pid);
        }

        if (cfg->max_samples > 0 && (unsigned int)collected >= cfg->max_samples)
            break;

        sleep_ms(cfg->interval_ms);
    }

    return collected;
}
