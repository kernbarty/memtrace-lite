#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/anomaly_report.h"
#include "../src/anomaly_sink.h"

static int custom_called = 0;
static AnomalySeverity last_sev;

static void custom_sink_fn(const AnomalyReport *r, void *ud)
{
    (void)ud;
    custom_called++;
    last_sev = r->severity;
}

static void test_severity_classification(void)
{
    assert(anomaly_report_severity(5.0)  == ANOMALY_SEV_LOW);
    assert(anomaly_report_severity(25.0) == ANOMALY_SEV_MEDIUM);
    assert(anomaly_report_severity(60.0) == ANOMALY_SEV_HIGH);
    assert(anomaly_report_severity(-55.0) == ANOMALY_SEV_HIGH);
    printf("PASS: severity classification\n");
}

static void test_report_build(void)
{
    AnomalyResult result;
    result.is_anomaly = 1;
    result.z_score    = 3.5;

    AnomalyReport report;
    anomaly_report_build(&report, &result, 1234, 2000, 1000);

    assert(report.pid         == 1234);
    assert(report.current_kb  == 2000);
    assert(report.baseline_kb == 1000);
    assert(report.deviation_pct > 99.0 && report.deviation_pct < 101.0);
    assert(report.severity    == ANOMALY_SEV_HIGH);
    assert(strlen(report.description) > 0);
    printf("PASS: report build\n");
}

static void test_report_format(void)
{
    AnomalyResult result = { .is_anomaly = 1, .z_score = 2.1 };
    AnomalyReport report;
    anomaly_report_build(&report, &result, 42, 1200, 1000);

    char buf[256];
    int n = anomaly_report_format(&report, buf, sizeof(buf));
    assert(n > 0);
    assert(strstr(buf, "ANOMALY REPORT") != NULL);
    printf("PASS: report format\n");
}

static void test_sink_custom_dispatch(void)
{
    AnomalySink sink;
    anomaly_sink_init_custom(&sink, custom_sink_fn, NULL, ANOMALY_SEV_LOW);

    AnomalyResult result = { .is_anomaly = 1, .z_score = 4.0 };
    AnomalyReport report;
    anomaly_report_build(&report, &result, 99, 3000, 1000);

    custom_called = 0;
    anomaly_sink_dispatch(&sink, &report);
    assert(custom_called == 1);
    assert(last_sev == ANOMALY_SEV_HIGH);
    printf("PASS: custom sink dispatch\n");
}

static void test_sink_min_severity_filter(void)
{
    AnomalySink sink;
    anomaly_sink_init_custom(&sink, custom_sink_fn, NULL, ANOMALY_SEV_HIGH);

    AnomalyResult result = { .is_anomaly = 1, .z_score = 1.5 };
    AnomalyReport report;
    anomaly_report_build(&report, &result, 7, 1050, 1000); /* ~5% deviation -> LOW */

    custom_called = 0;
    anomaly_sink_dispatch(&sink, &report);
    assert(custom_called == 0); /* filtered out */
    printf("PASS: sink min_severity filter\n");
}

int main(void)
{
    test_severity_classification();
    test_report_build();
    test_report_format();
    test_sink_custom_dispatch();
    test_sink_min_severity_filter();
    printf("All anomaly_report tests passed.\n");
    return 0;
}
