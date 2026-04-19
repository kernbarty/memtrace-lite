#ifndef ANOMALY_SINK_H
#define ANOMALY_SINK_H

#include "anomaly_report.h"
#include <stdio.h>

/* Sink types */
typedef enum {
    SINK_STDERR = 0,
    SINK_FILE   = 1,
    SINK_CUSTOM = 2
} AnomalySinkType;

typedef void (*AnomalySinkFn)(const AnomalyReport *report, void *userdata);

typedef struct {
    AnomalySinkType type;
    FILE           *file;      /* used when type == SINK_FILE   */
    AnomalySinkFn   fn;        /* used when type == SINK_CUSTOM */
    void           *userdata;
    AnomalySeverity min_severity; /* reports below this level are dropped */
} AnomalySink;

void anomaly_sink_init_stderr(AnomalySink *sink, AnomalySeverity min_sev);
void anomaly_sink_init_file(AnomalySink *sink, FILE *f, AnomalySeverity min_sev);
void anomaly_sink_init_custom(AnomalySink *sink, AnomalySinkFn fn,
                              void *userdata, AnomalySeverity min_sev);

/* Dispatch a report to the sink (noop if below min_severity) */
void anomaly_sink_dispatch(AnomalySink *sink, const AnomalyReport *report);

#endif /* ANOMALY_SINK_H */
