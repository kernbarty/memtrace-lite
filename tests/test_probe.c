#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/probe.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define CHECK(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; printf("  PASS: %s\n", msg); } \
    else       { printf("  FAIL: %s  (line %d)\n", msg, __LINE__); } \
} while(0)

static void test_probe_init(void)
{
    puts("test_probe_init");
    Probe p;
    int rc = probe_init(&p, "test_rss", "unit", PROBE_KIND_RSS);
    CHECK(rc == 0,               "init returns 0");
    CHECK(p.name[0] != '\0',     "name is set");
    CHECK(strcmp(p.tag, "unit") == 0, "tag is set");
    CHECK(p.kind == PROBE_KIND_RSS,   "kind is RSS");
    CHECK(p.value_kb > 0,        "value_kb > 0 after init");
    CHECK(p.delta_kb == 0,       "delta is 0 right after init");
}

static void test_probe_null(void)
{
    puts("test_probe_null");
    CHECK(probe_init(NULL, "x", NULL, PROBE_KIND_RSS) != 0, "NULL probe rejected");
    CHECK(probe_init((Probe*)1, NULL, NULL, PROBE_KIND_RSS) != 0, "NULL name rejected");
    CHECK(probe_sample(NULL) != 0, "sample NULL rejected");
    CHECK(probe_age_ms(NULL) < 0,  "age NULL returns negative");
}

static void test_probe_sample(void)
{
    puts("test_probe_sample");
    Probe p;
    probe_init(&p, "sample_test", "grp", PROBE_KIND_VSZ);
    uint64_t before = p.value_kb;
    int rc = probe_sample(&p);
    CHECK(rc == 0,          "sample returns 0");
    CHECK(p.value_kb > 0,   "value_kb > 0 after sample");
    /* delta should equal current - baseline */
    int64_t expected = (int64_t)p.value_kb - (int64_t)p.baseline_kb;
    CHECK(p.delta_kb == expected, "delta_kb consistent with baseline");
    (void)before;
}

static void test_probe_reset_baseline(void)
{
    puts("test_probe_reset_baseline");
    Probe p;
    probe_init(&p, "baseline_test", NULL, PROBE_KIND_RSS);
    probe_sample(&p);
    probe_reset_baseline(&p);
    CHECK(p.baseline_kb == p.value_kb, "baseline updated to current value");
    CHECK(p.delta_kb == 0,             "delta reset to 0");
}

static void test_probe_format(void)
{
    puts("test_probe_format");
    Probe p;
    probe_init(&p, "fmt_probe", "fmt", PROBE_KIND_HEAP);
    char buf[256];
    char *ret = probe_format(&p, buf, sizeof(buf));
    CHECK(ret == buf,                 "format returns buf");
    CHECK(strstr(buf, "fmt_probe") != NULL, "name in output");
    CHECK(strstr(buf, "heap")      != NULL, "kind in output");
    CHECK(strstr(buf, "fmt")       != NULL, "tag in output");
    /* NULL safety */
    CHECK(probe_format(NULL, buf, sizeof(buf)) == buf, "NULL probe safe");
}

static void test_probe_age(void)
{
    puts("test_probe_age");
    Probe p;
    probe_init(&p, "age_probe", NULL, PROBE_KIND_RSS);
    long age = probe_age_ms(&p);
    CHECK(age >= 0, "age is non-negative");
}

int main(void)
{
    puts("=== test_probe ===");
    test_probe_init();
    test_probe_null();
    test_probe_sample();
    test_probe_reset_baseline();
    test_probe_format();
    test_probe_age();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
