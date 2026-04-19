#ifndef PRESSURE_H
#define PRESSURE_H

#include <stdint.h>
#include <stdbool.h>

/* Memory pressure levels */
typedef enum {
    PRESSURE_NONE   = 0,
    PRESSURE_LOW    = 1,
    PRESSURE_MEDIUM = 2,
    PRESSURE_HIGH   = 3,
    PRESSURE_CRITICAL = 4
} pressure_level_t;

typedef struct {
    uint64_t rss_kb;
    uint64_t vms_kb;
    float    usage_ratio;   /* rss / available */
    pressure_level_t level;
    uint64_t timestamp_ms;
} pressure_sample_t;

typedef struct {
    uint64_t low_threshold_kb;
    uint64_t medium_threshold_kb;
    uint64_t high_threshold_kb;
    uint64_t critical_threshold_kb;
} pressure_config_t;

void pressure_config_default(pressure_config_t *cfg);
pressure_level_t pressure_evaluate(const pressure_config_t *cfg, uint64_t rss_kb);
const char *pressure_level_str(pressure_level_t level);
bool pressure_sample_fill(pressure_sample_t *s, pid_t pid, const pressure_config_t *cfg);

#endif /* PRESSURE_H */
