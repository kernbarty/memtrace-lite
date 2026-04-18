#ifndef ALERT_H
#define ALERT_H

#include <stddef.h>

/* Alert levels for memory threshold violations */
typedef enum {
    ALERT_LEVEL_INFO    = 0,
    ALERT_LEVEL_WARNING = 1,
    ALERT_LEVEL_CRITICAL = 2
} AlertLevel;

/* Alert configuration thresholds (in KB) */
typedef struct {
    size_t warning_kb;   /* RSS threshold for WARNING */
    size_t critical_kb;  /* RSS threshold for CRITICAL */
} AlertConfig;

/* Represents a triggered alert */
typedef struct {
    AlertLevel  level;
    size_t      rss_kb;
    size_t      threshold_kb;
    long        timestamp_ms;
} Alert;

/* Initialize alert config with given thresholds */
void alert_config_init(AlertConfig *cfg, size_t warning_kb, size_t critical_kb);

/* Evaluate RSS value against config; returns ALERT_LEVEL_INFO if no violation */
AlertLevel alert_evaluate(const AlertConfig *cfg, size_t rss_kb);

/* Build an Alert struct for the given RSS and config */
Alert alert_build(const AlertConfig *cfg, size_t rss_kb);

/* Format alert into buf; returns bytes written */
int alert_format(const Alert *alert, char *buf, size_t buf_size);

#endif /* ALERT_H */
