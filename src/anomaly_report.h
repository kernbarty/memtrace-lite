#ifndef ANOMALY_REPORT_H
#define ANOMALY_REPORT_H

#include "anomaly.h"
#include <stddef.h>
#include <stdio.h>

/* Severity levels for anomaly reports */
typedef enum {
    ANOMALY_SEV_LOW    = 0,
    ANOMALY_SEV_MEDIUM = 1,
    ANOMALY_SEV_HIGH   = 2
} AnomalySeverity;

typedef struct {
    AnomalyResult   result;
    AnomalySeverity severity;
    long            pid;
    unsigned long   current_kb;
    unsigned long   baseline_kb;
    double          deviation_pct;
    char            description[128];
} AnomalyReport;

/* Build a report from a raw anomaly result */
void anomaly_report_build(AnomalyReport *report,
                          const AnomalyResult *result,
                          long pid,
                          unsigned long current_kb,
                          unsigned long baseline_kb);

/* Classify severity based on deviation percentage */
AnomalySeverity anomaly_report_severity(double deviation_pct);

/* Print report to file stream */
void anomaly_report_print(const AnomalyReport *report, FILE *out);

/* Format report into caller-supplied buffer; returns bytes written */
int anomaly_report_format(const AnomalyReport *report, char *buf, size_t len);

#endif /* ANOMALY_REPORT_H */
