#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/escalate.h"

static escalate_config_t make_cfg(uint32_t notice, uint32_t warning, uint32_t critical, uint64_t auto_reset_ms) {
    escalate_config_t cfg;
    cfg.notice_threshold   = notice;
    cfg.warning_threshold  = warning;
    cfg.critical_threshold = critical;
    cfg.auto_reset_ms      = auto_reset_ms;
    return cfg;
}

static void test_initial_state(void) {
    escalate_t e;
    escalate_config_t cfg = make_cfg(2, 4, 6, 0);
    escalate_init(&e, &cfg);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NONE);
    assert(escalate_trigger_count(&e) == 0);
    printf("PASS test_initial_state\n");
}

static void test_level_progression(void) {
    escalate_t e;
    escalate_config_t cfg = make_cfg(2, 4, 6, 0);
    escalate_init(&e, &cfg);

    escalate_level_t lvl;
    lvl = escalate_update(&e, 1, 1000);
    assert(lvl == ESCALATE_LEVEL_NONE);

    lvl = escalate_update(&e, 1, 2000);
    assert(lvl == ESCALATE_LEVEL_NOTICE);

    lvl = escalate_update(&e, 1, 3000);
    assert(lvl == ESCALATE_LEVEL_NOTICE);

    lvl = escalate_update(&e, 1, 4000);
    assert(lvl == ESCALATE_LEVEL_WARNING);

    lvl = escalate_update(&e, 1, 5000);
    assert(lvl == ESCALATE_LEVEL_WARNING);

    lvl = escalate_update(&e, 1, 6000);
    assert(lvl == ESCALATE_LEVEL_CRITICAL);

    assert(escalate_trigger_count(&e) == 6);
    printf("PASS test_level_progression\n");
}

static void test_no_trigger_holds_level(void) {
    escalate_t e;
    escalate_config_t cfg = make_cfg(1, 3, 5, 0);
    escalate_init(&e, &cfg);

    escalate_update(&e, 1, 1000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NOTICE);

    escalate_update(&e, 0, 2000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NOTICE);
    printf("PASS test_no_trigger_holds_level\n");
}

static void test_auto_reset(void) {
    escalate_t e;
    escalate_config_t cfg = make_cfg(1, 3, 5, 5000);
    escalate_init(&e, &cfg);

    escalate_update(&e, 1, 1000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NOTICE);

    /* Not enough time passed — level stays */
    escalate_update(&e, 0, 4000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NOTICE);

    /* Enough time passed — auto reset */
    escalate_update(&e, 0, 7000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NONE);
    assert(escalate_trigger_count(&e) == 0);
    printf("PASS test_auto_reset\n");
}

static void test_manual_reset(void) {
    escalate_t e;
    escalate_config_t cfg = make_cfg(1, 2, 3, 0);
    escalate_init(&e, &cfg);

    escalate_update(&e, 1, 1000);
    escalate_update(&e, 1, 2000);
    escalate_update(&e, 1, 3000);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_CRITICAL);

    escalate_reset(&e);
    assert(escalate_current_level(&e) == ESCALATE_LEVEL_NONE);
    assert(escalate_trigger_count(&e) == 0);
    printf("PASS test_manual_reset\n");
}

static void test_level_name(void) {
    assert(strcmp(escalate_level_name(ESCALATE_LEVEL_NONE),     "none")     == 0);
    assert(strcmp(escalate_level_name(ESCALATE_LEVEL_NOTICE),   "notice")   == 0);
    assert(strcmp(escalate_level_name(ESCALATE_LEVEL_WARNING),  "warning")  == 0);
    assert(strcmp(escalate_level_name(ESCALATE_LEVEL_CRITICAL), "critical") == 0);
    printf("PASS test_level_name\n");
}

int main(void) {
    test_initial_state();
    test_level_progression();
    test_no_trigger_holds_level();
    test_auto_reset();
    test_manual_reset();
    test_level_name();
    printf("All escalate tests passed.\n");
    return 0;
}
