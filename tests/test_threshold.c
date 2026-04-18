#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/threshold.h"

static void test_init_free(void) {
    threshold_set_t ts;
    assert(threshold_set_init(&ts, 8) == 0);
    assert(ts.capacity == 8);
    assert(ts.count == 0);
    threshold_set_free(&ts);
    assert(ts.entries == NULL);
    printf("PASS test_init_free\n");
}

static void test_add_and_check_triggered(void) {
    threshold_set_t ts;
    threshold_set_init(&ts, 4);

    assert(threshold_add(&ts, THRESHOLD_RSS, THRESHOLD_WARN, 512000, "rss-warn") == 0);
    assert(threshold_add(&ts, THRESHOLD_RSS, THRESHOLD_CRITICAL, 1024000, "rss-crit") == 0);

    threshold_level_t lvl;
    const char *label;

    /* below all thresholds */
    assert(!threshold_check(&ts, THRESHOLD_RSS, 100000, &lvl, &label));

    /* above warn only */
    assert(threshold_check(&ts, THRESHOLD_RSS, 600000, &lvl, &label));
    assert(lvl == THRESHOLD_WARN);
    assert(strcmp(label, "rss-warn") == 0);

    /* above both — should return critical */
    assert(threshold_check(&ts, THRESHOLD_RSS, 2000000, &lvl, &label));
    assert(lvl == THRESHOLD_CRITICAL);
    assert(strcmp(label, "rss-crit") == 0);

    threshold_set_free(&ts);
    printf("PASS test_add_and_check_triggered\n");
}

static void test_metric_isolation(void) {
    threshold_set_t ts;
    threshold_set_init(&ts, 4);
    threshold_add(&ts, THRESHOLD_VSZ, THRESHOLD_WARN, 200000, "vsz-warn");

    threshold_level_t lvl;
    const char *label;
    /* RSS value should not trigger VSZ threshold */
    assert(!threshold_check(&ts, THRESHOLD_RSS, 999999, &lvl, &label));
    assert(threshold_check(&ts, THRESHOLD_VSZ, 200000, &lvl, &label));

    threshold_set_free(&ts);
    printf("PASS test_metric_isolation\n");
}

static void test_capacity_limit(void) {
    threshold_set_t ts;
    threshold_set_init(&ts, 2);
    assert(threshold_add(&ts, THRESHOLD_RSS, THRESHOLD_WARN, 1000, "a") == 0);
    assert(threshold_add(&ts, THRESHOLD_RSS, THRESHOLD_WARN, 2000, "b") == 0);
    assert(threshold_add(&ts, THRESHOLD_RSS, THRESHOLD_WARN, 3000, "c") == -1);
    threshold_set_free(&ts);
    printf("PASS test_capacity_limit\n");
}

int main(void) {
    test_init_free();
    test_add_and_check_triggered();
    test_metric_isolation();
    test_capacity_limit();
    printf("All threshold tests passed.\n");
    return 0;
}
