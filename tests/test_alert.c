#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../src/alert.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while (0)

static void test_config_init(void) {
    AlertConfig cfg;
    alert_config_init(&cfg, 512, 1024);
    assert(cfg.warning_kb  == 512);
    assert(cfg.critical_kb == 1024);
    printf("  PASS test_config_init\n");
}

static void test_evaluate_info(void) {
    AlertConfig cfg;
    alert_config_init(&cfg, 512, 1024);
    assert(alert_evaluate(&cfg, 256) == ALERT_LEVEL_INFO);
    printf("  PASS test_evaluate_info\n");
}

static void test_evaluate_warning(void) {
    AlertConfig cfg;
    alert_config_init(&cfg, 512, 1024);
    assert(alert_evaluate(&cfg, 512)  == ALERT_LEVEL_WARNING);
    assert(alert_evaluate(&cfg, 800)  == ALERT_LEVEL_WARNING);
    printf("  PASS test_evaluate_warning\n");
}

static void test_evaluate_critical(void) {
    AlertConfig cfg;
    alert_config_init(&cfg, 512, 1024);
    assert(alert_evaluate(&cfg, 1024) == ALERT_LEVEL_CRITICAL);
    assert(alert_evaluate(&cfg, 2048) == ALERT_LEVEL_CRITICAL);
    printf("  PASS test_evaluate_critical\n");
}

static void test_build_sets_threshold(void) {
    AlertConfig cfg;
    alert_config_init(&cfg, 512, 1024);

    Alert a = alert_build(&cfg, 600);
    assert(a.level        == ALERT_LEVEL_WARNING);
    assert(a.threshold_kb == 512);
    assert(a.rss_kb       == 600);

    Alert b = alert_build(&cfg, 1500);
    assert(b.level        == ALERT_LEVEL_CRITICAL);
    assert(b.threshold_kb == 1024);
    printf("  PASS test_build_sets_threshold\n");
}

static void test_format_output(void) {
    AlertConfig cfg;
    alert_config_init(&cfg, 512, 1024);
    Alert a = alert_build(&cfg, 700);

    char buf[256];
    int n = alert_format(&a, buf, sizeof(buf));
    assert(n > 0);
    assert(strstr(buf, "WARNING") != NULL);
    assert(strstr(buf, "700")     != NULL);
    printf("  PASS test_format_output\n");
}

int main(void) {
    printf("Running alert tests...\n");
    RUN_TEST(test_config_init);
    RUN_TEST(test_evaluate_info);
    RUN_TEST(test_evaluate_warning);
    RUN_TEST(test_evaluate_critical);
    RUN_TEST(test_build_sets_threshold);
    RUN_TEST(test_format_output);
    printf("%d/%d tests passed.\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
