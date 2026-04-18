#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/reporter.h"
#include "../src/snapshot.h"

static MemSnapshot make_snap(long ts, long vm, long rss, long shared) {
    MemSnapshot s;
    s.timestamp_ms  = ts;
    s.vm_size_kb    = vm;
    s.vm_rss_kb     = rss;
    s.vm_shared_kb  = shared;
    return s;
}

static void test_text_output(void) {
    ReporterConfig cfg = { REPORT_FORMAT_TEXT, NULL, 0 };
    reporter_init(&cfg);
    assert(cfg.output == stdout);

    FILE *tmp = tmpfile();
    assert(tmp);
    cfg.output = tmp;

    MemSnapshot snap = make_snap(1000, 20480, 8192, 512);
    reporter_print_snapshot(&cfg, &snap);

    rewind(tmp);
    char buf[256];
    fgets(buf, sizeof(buf), tmp);
    assert(strstr(buf, "VmSize") != NULL);
    assert(strstr(buf, "VmRSS")  != NULL);
    fclose(tmp);
    printf("PASS: test_text_output\n");
}

static void test_csv_output(void) {
    FILE *tmp = tmpfile();
    assert(tmp);
    ReporterConfig cfg = { REPORT_FORMAT_CSV, tmp, 0 };
    reporter_init(&cfg);

    reporter_print_csv_header(&cfg);
    MemSnapshot snap = make_snap(2000, 40960, 16384, 1024);
    reporter_print_snapshot(&cfg, &snap);

    rewind(tmp);
    char buf[256];
    fgets(buf, sizeof(buf), tmp);  /* header */
    assert(strstr(buf, "timestamp_ms") != NULL);
    fgets(buf, sizeof(buf), tmp);  /* data row */
    assert(strstr(buf, "2000") != NULL);
    fclose(tmp);
    printf("PASS: test_csv_output\n");
}

static void test_delta_output(void) {
    FILE *tmp = tmpfile();
    assert(tmp);
    ReporterConfig cfg = { REPORT_FORMAT_TEXT, tmp, 1 };

    MemSnapshot prev = make_snap(0,    20480, 8192, 512);
    MemSnapshot curr = make_snap(1000, 24576, 9216, 512);
    reporter_print_delta(&cfg, &prev, &curr);

    rewind(tmp);
    char buf[256];
    fgets(buf, sizeof(buf), tmp);
    assert(strstr(buf, "Delta") != NULL);
    assert(strstr(buf, "+4096") != NULL);
    fclose(tmp);
    printf("PASS: test_delta_output\n");
}

int main(void) {
    test_text_output();
    test_csv_output();
    test_delta_output();
    printf("All reporter tests passed.\n");
    return 0;
}
