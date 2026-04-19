#include "pressure.h"
#include "procmem.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static uint64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}

static uint64_t available_kb(void) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return 1;
    char line[128];
    uint64_t avail = 1;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line + 13, "%llu", (unsigned long long *)&avail);
            break;
        }
    }
    fclose(f);
    return avail ? avail : 1;
}

void pressure_config_default(pressure_config_t *cfg) {
    cfg->low_threshold_kb      =  256 * 1024;
    cfg->medium_threshold_kb   =  512 * 1024;
    cfg->high_threshold_kb     = 1024 * 1024;
    cfg->critical_threshold_kb = 2048 * 1024;
}

pressure_level_t pressure_evaluate(const pressure_config_t *cfg, uint64_t rss_kb) {
    if (rss_kb >= cfg->critical_threshold_kb) return PRESSURE_CRITICAL;
    if (rss_kb >= cfg->high_threshold_kb)     return PRESSURE_HIGH;
    if (rss_kb >= cfg->medium_threshold_kb)   return PRESSURE_MEDIUM;
    if (rss_kb >= cfg->low_threshold_kb)      return PRESSURE_LOW;
    return PRESSURE_NONE;
}

const char *pressure_level_str(pressure_level_t level) {
    switch (level) {
        case PRESSURE_NONE:     return "none";
        case PRESSURE_LOW:      return "low";
        case PRESSURE_MEDIUM:   return "medium";
        case PRESSURE_HIGH:     return "high";
        case PRESSURE_CRITICAL: return "critical";
        default:                return "unknown";
    }
}

bool pressure_sample_fill(pressure_sample_t *s, pid_t pid, const pressure_config_t *cfg) {
    mem_info_t info;
    if (procmem_read(pid, &info) != 0) return false;
    s->rss_kb       = info.rss_kb;
    s->vms_kb       = info.vms_kb;
    s->usage_ratio  = (float)info.rss_kb / (float)available_kb();
    s->level        = pressure_evaluate(cfg, info.rss_kb);
    s->timestamp_ms = now_ms();
    return true;
}
