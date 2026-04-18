#include "alert.h"

#include <stdio.h>
#include <time.h>

static long current_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

void alert_config_init(AlertConfig *cfg, size_t warning_kb, size_t critical_kb) {
    if (!cfg) return;
    cfg->warning_kb  = warning_kb;
    cfg->critical_kb = critical_kb;
}

AlertLevel alert_evaluate(const AlertConfig *cfg, size_t rss_kb) {
    if (!cfg) return ALERT_LEVEL_INFO;
    if (rss_kb >= cfg->critical_kb) return ALERT_LEVEL_CRITICAL;
    if (rss_kb >= cfg->warning_kb)  return ALERT_LEVEL_WARNING;
    return ALERT_LEVEL_INFO;
}

Alert alert_build(const AlertConfig *cfg, size_t rss_kb) {
    Alert a;
    a.level        = alert_evaluate(cfg, rss_kb);
    a.rss_kb       = rss_kb;
    a.timestamp_ms = current_ms();

    switch (a.level) {
        case ALERT_LEVEL_CRITICAL:
            a.threshold_kb = cfg ? cfg->critical_kb : 0;
            break;
        case ALERT_LEVEL_WARNING:
            a.threshold_kb = cfg ? cfg->warning_kb : 0;
            break;
        default:
            a.threshold_kb = 0;
            break;
    }
    return a;
}

int alert_format(const Alert *alert, char *buf, size_t buf_size) {
    if (!alert || !buf || buf_size == 0) return -1;

    static const char *level_str[] = { "INFO", "WARNING", "CRITICAL" };
    const char *lvl = level_str[alert->level];

    return snprintf(buf, buf_size,
        "[ALERT][%s] rss=%zu KB threshold=%zu KB ts=%ld ms",
        lvl, alert->rss_kb, alert->threshold_kb, alert->timestamp_ms);
}
