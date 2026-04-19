#include "anomaly.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

struct anomaly_ctx {
    anomaly_config_t cfg;
};

void anomaly_default_config(anomaly_config_t *cfg) {
    if (!cfg) return;
    cfg->spike_zscore_threshold = 3.0;
    cfg->drop_zscore_threshold  = 3.0;
    cfg->trend_slope_threshold  = 512.0;
}

anomalY_ctx_t *anomaly_create(const anomaly_config_t *cfg) {
    anomaly_ctx_t *ctx = calloc(1, sizeof(*ctx));
    if (!ctx) return NULL;
    if (cfg) {
        ctx->cfg = *cfg;
    } else {
        anomaly_default_config(&ctx->cfg);
    }
    return ctx;
}

void anomaly_destroy(anomaly_ctx_t *ctx) {
    free(ctx);
}

static uint64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}

anomaly_event_t anomaly_feed(anomaly_ctx_t *ctx, size_t value_kb,
                             double mean_kb, double stddev_kb,
                             double slope_kb_per_sample,
                             uint64_t timestamp) {
    anomaly_event_t ev;
    memset(&ev, 0, sizeof(ev));
    ev.type        = ANOMALY_NONE;
    ev.value_kb    = value_kb;
    ev.baseline_kb = (size_t)mean_kb;
    ev.timestamp   = timestamp ? timestamp : now_ms();

    if (stddev_kb > 0.0) {
        double zscore = ((double)value_kb - mean_kb) / stddev_kb;
        ev.zscore = zscore;

        if (zscore > ctx->cfg.spike_zscore_threshold) {
            ev.type = ANOMALY_SPIKE;
            return ev;
        }
        if (zscore < -ctx->cfg.drop_zscore_threshold) {
            ev.type = ANOMALY_DROP;
            return ev;
        }
    }

    if (fabs(slope_kb_per_sample) > ctx->cfg.trend_slope_threshold) {
        ev.type = ANOMALY_TREND;
    }

    return ev;
}
