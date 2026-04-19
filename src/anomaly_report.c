#include "anomaly_report.h"
#include <stdio.h>
#include <string.h>

AnomalySeverity anomaly_report_severity(double deviation_pct)
{
    if (deviation_pct < 0.0) deviation_pct = -deviation_pct;
    if (deviation_pct >= 50.0) return ANOMALY_SEV_HIGH;
    if (deviation_pct >= 20.0) return ANOMALY_SEV_MEDIUM;
    return ANOMALY_SEV_LOW;
}

void anomaly_report_build(AnomalyReport *report,
                          const AnomalyResult *result,
                          long pid,
                          unsigned long current_kb,
                          unsigned long baseline_kb)
{
    if (!report || !result) return;

    memset(report, 0, sizeof(*report));
    report->result      = *result;
    report->pid         = pid;
    report->current_kb  = current_kb;
    report->baseline_kb = baseline_kb;

    if (baseline_kb > 0)
        report->deviation_pct =
            100.0 * ((double)current_kb - (double)baseline_kb) / (double)baseline_kb;
    else
        report->deviation_pct = 0.0;

    report->severity = anomaly_report_severity(report->deviation_pct);

    const char *sev_str[] = { "LOW", "MEDIUM", "HIGH" };
    snprintf(report->description, sizeof(report->description),
             "pid=%ld current=%lukB baseline=%lukB deviation=%.1f%% severity=%s",
             pid, current_kb, baseline_kb,
             report->deviation_pct,
             sev_str[report->severity]);
}

void anomaly_report_print(const AnomalyReport *report, FILE *out)
{
    if (!report || !out) return;
    fprintf(out, "[ANOMALY REPORT] %s\n", report->description);
}

int anomaly_report_format(const AnomalyReport *report, char *buf, size_t len)
{
    if (!report || !buf || len == 0) return -1;
    return snprintf(buf, len, "[ANOMALY REPORT] %s", report->description);
}
