#include "anomaly_sink.h"
#include <stdio.h>

void anomaly_sink_init_stderr(AnomalySink *sink, AnomalySeverity min_sev)
{
    if (!sink) return;
    sink->type         = SINK_STDERR;
    sink->file         = stderr;
    sink->fn           = NULL;
    sink->userdata     = NULL;
    sink->min_severity = min_sev;
}

void anomaly_sink_init_file(AnomalySink *sink, FILE *f, AnomalySeverity min_sev)
{
    if (!sink) return;
    sink->type         = SINK_FILE;
    sink->file         = f;
    sink->fn           = NULL;
    sink->userdata     = NULL;
    sink->min_severity = min_sev;
}

void anomaly_sink_init_custom(AnomalySink *sink, AnomalySinkFn fn,
                              void *userdata, AnomalySeverity min_sev)
{
    if (!sink) return;
    sink->type         = SINK_CUSTOM;
    sink->file         = NULL;
    sink->fn           = fn;
    sink->userdata     = userdata;
    sink->min_severity = min_sev;
}

void anomaly_sink_dispatch(AnomalySink *sink, const AnomalyReport *report)
{
    if (!sink || !report) return;
    if (report->severity < sink->min_severity) return;

    switch (sink->type) {
    case SINK_STDERR:
    case SINK_FILE:
        if (sink->file)
            anomaly_report_print(report, sink->file);
        break;
    case SINK_CUSTOM:
        if (sink->fn)
            sink->fn(report, sink->userdata);
        break;
    }
}
