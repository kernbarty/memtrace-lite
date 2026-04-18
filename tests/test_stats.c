#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../src/stats.h"

static void test_init(void)
{
    MemStats s;
    stats_init(&s);
    assert(s.sample_count == 0);
    assert(s.peak_rss_kb == 0);
    printf("PASS test_init\n");
}

static void test_single_sample(void)
{
    MemStats s;
    stats_init(&s);
    stats_update(&s, 1024, 4096);
    stats_finalise(&s);

    assert(s.sample_count == 1);
    assert(s.min_rss_kb == 1024);
    assert(s.max_rss_kb == 1024);
    assert(s.peak_rss_kb == 1024);
    assert(s.min_vms_kb == 4096);
    assert(s.max_vms_kb == 4096);
    assert(fabs(s.avg_rss_kb - 1024.0) < 0.01);
    printf("PASS test_single_sample\n");
}

static void test_multiple_samples(void)
{
    MemStats s;
    stats_init(&s);
    stats_update(&s, 100, 500);
    stats_update(&s, 300, 700);
    stats_update(&s, 200, 600);
    stats_finalise(&s);

    assert(s.sample_count == 3);
    assert(s.min_rss_kb == 100);
    assert(s.max_rss_kb == 300);
    assert(s.peak_rss_kb == 300);
    assert(fabs(s.avg_rss_kb - 200.0) < 0.01);
    assert(s.min_vms_kb == 500);
    assert(s.max_vms_kb == 700);
    assert(fabs(s.avg_vms_kb - 600.0) < 0.01);
    printf("PASS test_multiple_samples\n");
}

static void test_null_safety(void)
{
    stats_init(NULL);
    stats_update(NULL, 100, 200);
    stats_finalise(NULL);
    printf("PASS test_null_safety\n");
}

int main(void)
{
    test_init();
    test_single_sample();
    test_multiple_samples();
    test_null_safety();
    printf("All stats tests passed.\n");
    return 0;
}
