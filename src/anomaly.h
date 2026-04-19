#ifndef ANOMALY_H
#define ANOMALY_H

#include <stddef.h>
#include <stdint.h>

/* Anomaly detection based on z-score deviation from baseline */

typedef enum {
    ANOMALY_NONE   = 0,
    ANOMALY_SPIKE  = 1,  /* sudden increase */
    ANOMALY_DROP   = 2,  /* sudden decrease */
    ANOMALY_TREND  = 3   /* sustained drift */
} anomaly_type_t;

typedef struct {
    anomaly_type_t type;
    double         zscore;       /* how many std devs from mean */
    size_t         value_kb;     /* observed value */
    size_t         baseline_kb;  /* expected value */
    uint64_t       timestamp;
} anomaly_event_t;

typedef struct {
    double spike_zscore_threshold;  /* default 3.0 */
    double drop_zscore_threshold;   /* default 3.0 */
    double trend_slope_threshold;   /* KB/sample, default 512 */
} anomaly_config_t;

typedef struct anomaly_ctx anomaly_ctx_t;

anomalY_ctx_t  *anomaly_create(const anomaly_config_t *cfg);
void            anomaly_destroy(anomaly_ctx_t *ctx);

/* Feed a new sample; returns detected anomaly (type==NONE if ok) */
anomaly_event_t anomaly_feed(anomaly_ctx_t *ctx, size_t value_kb,
                             double mean_kb, double stddev_kb,
                             double slope_kb_per_sample,
                             uint64_t timestamp);

void anomaly_default_config(anomaly_config_t *cfg);

#endif /* ANOMALY_H */
