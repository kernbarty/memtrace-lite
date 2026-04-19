#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "../src/pressure.h"

static void test_config_default(void) {
    pressure_config_t cfg;
    pressure_config_default(&cfg);
    assert(cfg.low_threshold_kb      > 0);
    assert(cfg.medium_threshold_kb   > cfg.low_threshold_kb);
    assert(cfg.high_threshold_kb     > cfg.medium_threshold_kb);
    assert(cfg.critical_threshold_kb > cfg.high_threshold_kb);
    printf("PASS test_config_default\n");
}

static void test_evaluate_levels(void) {
    pressure_config_t cfg;
    pressure_config_default(&cfg);

    assert(pressure_evaluate(&cfg, 0) == PRESSURE_NONE);
    assert(pressure_evaluate(&cfg, cfg.low_threshold_kb)      == PRESSURE_LOW);
    assert(pressure_evaluate(&cfg, cfg.medium_threshold_kb)   == PRESSURE_MEDIUM);
    assert(pressure_evaluate(&cfg, cfg.high_threshold_kb)     == PRESSURE_HIGH);
    assert(pressure_evaluate(&cfg, cfg.critical_threshold_kb) == PRESSURE_CRITICAL);
    printf("PASS test_evaluate_levels\n");
}

static void test_level_str(void) {
    assert(pressure_level_str(PRESSURE_NONE)     != NULL);
    assert(pressure_level_str(PRESSURE_LOW)      != NULL);
    assert(pressure_level_str(PRESSURE_MEDIUM)   != NULL);
    assert(pressure_level_str(PRESSURE_HIGH)     != NULL);
    assert(pressure_level_str(PRESSURE_CRITICAL) != NULL);
    printf("PASS test_level_str\n");
}

static void test_sample_fill_self(void) {
    pressure_config_t cfg;
    pressure_config_default(&cfg);
    pressure_sample_t s;
    pid_t pid = getpid();
    bool ok = pressure_sample_fill(&s, pid, &cfg);
    assert(ok);
    assert(s.rss_kb > 0);
    assert(s.vms_kb > 0);
    assert(s.usage_ratio > 0.0f);
    assert(s.timestamp_ms > 0);
    printf("PASS test_sample_fill_self (rss=%llu KB, level=%s)\n",
           (unsigned long long)s.rss_kb, pressure_level_str(s.level));
}

static void test_sample_fill_invalid_pid(void) {
    pressure_config_t cfg;
    pressure_config_default(&cfg);
    pressure_sample_t s;
    bool ok = pressure_sample_fill(&s, 99999999, &cfg);
    assert(!ok);
    printf("PASS test_sample_fill_invalid_pid\n");
}

int main(void) {
    test_config_default();
    test_evaluate_levels();
    test_level_str();
    test_sample_fill_self();
    test_sample_fill_invalid_pid();
    printf("All pressure tests passed.\n");
    return 0;
}
