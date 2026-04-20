#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/escalate.h"

static escalate_config_t default_cfg(void) {
    escalate_config_t cfg;
    cfg.warn_strikes     = 2;
    cfg.error_strikes    = 4;
    cfg.critical_strikes = 6;
    cfg.reset_strikes    = 3;
    return cfg;
}

static void test_initial_state(void) {
    escalate_t e;
    escalate_config_t cfg = default_cfg();
    escalate_init(&e, &cfg);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NONE);
    printf("PASS: test_initial_state\n");
}

static void test_escalation_to_warn(void) {
    escalate_t e;
    escalate_config_t cfg = default_cfg();
    escalate_init(&e, &cfg);

    escalate_record_violation(&e, 1000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NONE);
    escalate_record_violation(&e, 2000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_WARN);
    printf("PASS: test_escalation_to_warn\n");
}

static void test_escalation_to_error(void) {
    escalate_t e;
    escalate_config_t cfg = default_cfg();
    escalate_init(&e, &cfg);

    for (int i = 0; i < 4; i++)
        escalate_record_violation(&e, (uint64_t)(i + 1) * 1000);

    assert(escalate_current_level(&e) == ESCALATE_LEVEL_ERROR);
    printf("PASS: test_escalation_to_error\n");
}

static void test_escalation_to_critical(void) {
    escalate_t e;
    escalate_config_t cfg = default_cfg();
    escalate_init(&e, &cfg);

    for (int i = 0; i < 6; i++)
        escalate_record_violation(&e, (uint64_t)(i + 1) * 1000);

    assert(escalate_current_level(&e) == ESCALATE_LEVEL_CRITICAL);
    printf("PASS: test_escalation_to_critical\n");
}

static void test_clear_resets_after_threshold(void) {
    escalate_t e;
    escalate_config_t cfg = default_cfg();
    escalate_init(&e, &cfg);

    for (int i = 0; i < 4; i++)
        escalate_record_violation(&e, (uint64_t)(i + 1) * 1000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_ERROR);

    escalate_record_clear(&e, 5000);
    escalate_record_clear(&e, 6000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_ERROR); /* not yet reset */

    escalate_record_clear(&e, 7000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NONE);
    printf("PASS: test_clear_resets_after_threshold\n");
}

static void test_violation_interrupts_clear_streak(void) {
    escalate_t e;
    escalate_config_t cfg = default_cfg();
    escalate_init(&e, &cfg);

    for (int i = 0; i < 4; i++)
        escalate_record_violation(&e, (uint64_t)(i + 1) * 1000);

    escalate_record_clear(&e, 5000);
    escalate_record_clear(&e, 6000);
    escalate_record_violation(&e, 7000); /* interrupts clear streak */
    escalate_record_clear(&e, 8000);
    escalate_record_clear(&e, 9000);
    assert(escalate_current_level(&e) != ESCALATE_LEVEL_NONE);
    printf("PASS: test_violation_interrupts_clear_streak\n");
}

static void test_level_str(void) {
    assert(strcmp(escalate_level_str(ESCALATE_LEVEL_NONE),     "none")     == 0);
    assert(strcmp(escalate_level_str(ESCALATE_LEVEL_WARN),     "warn")     == 0);
    assert(strcmp(escalate_level_str(ESCALATE_LEVEL_ERROR),    "error")    == 0);
    assert(strcmp(escalate_level_str(ESCALATE_LEVEL_CRITICAL), "critical") == 0);
    printf("PASS: test_level_str\n");
}

int main(void) {
    test_initial_state();
    test_escalation_to_warn();
    test_escalation_to_error();
    test_escalation_to_critical();
    test_clear_resets_after_threshold();
    test_violation_interrupts_clear_streak();
    test_level_str();
    printf("All escalate tests passed.\n");
    return 0;
}
