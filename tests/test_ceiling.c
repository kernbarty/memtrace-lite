#include <stdio.h>
#include <assert.h>
#include "../src/ceiling.h"

static void test_no_breach(void) {
    Ceiling c;
    CeilingConfig cfg = { .hard_limit = 1000, .soft_limit = 800 };
    ceiling_init(&c, &cfg);

    CeilingResult r = ceiling_check(&c, 500);
    assert(!r.breached);
    assert(r.severity == CEILING_OK);
    assert(!ceiling_is_active(&c));
    assert(ceiling_breach_count(&c) == 0);
    printf("PASS test_no_breach\n");
}

static void test_soft_breach(void) {
    Ceiling c;
    CeilingConfig cfg = { .hard_limit = 1000, .soft_limit = 800 };
    ceiling_init(&c, &cfg);

    CeilingResult r = ceiling_check(&c, 850);
    assert(r.breached);
    assert(r.severity == CEILING_SOFT);
    assert(!ceiling_is_active(&c));
    assert(ceiling_breach_count(&c) == 0);
    printf("PASS test_soft_breach\n");
}

static void test_hard_breach(void) {
    Ceiling c;
    CeilingConfig cfg = { .hard_limit = 1000, .soft_limit = 800 };
    ceiling_init(&c, &cfg);

    CeilingResult r = ceiling_check(&c, 1200);
    assert(r.breached);
    assert(r.severity == CEILING_HARD);
    assert(ceiling_is_active(&c));
    assert(ceiling_breach_count(&c) == 1);
    assert(r.consecutive == 1);
    printf("PASS test_hard_breach\n");
}

static void test_consecutive_hard_breaches(void) {
    Ceiling c;
    CeilingConfig cfg = { .hard_limit = 1000, .soft_limit = 0 };
    ceiling_init(&c, &cfg);

    ceiling_check(&c, 1100);
    ceiling_check(&c, 1200);
    CeilingResult r = ceiling_check(&c, 1300);

    assert(r.consecutive == 3);
    assert(ceiling_breach_count(&c) == 3);
    printf("PASS test_consecutive_hard_breaches\n");
}

static void test_recovery_resets_consecutive(void) {
    Ceiling c;
    CeilingConfig cfg = { .hard_limit = 1000, .soft_limit = 0 };
    ceiling_init(&c, &cfg);

    ceiling_check(&c, 1100);
    ceiling_check(&c, 1200);
    ceiling_check(&c, 500);  /* recovery */
    assert(!ceiling_is_active(&c));

    CeilingResult r = ceiling_check(&c, 1050);
    assert(r.consecutive == 1);
    printf("PASS test_recovery_resets_consecutive\n");
}

static void test_reset(void) {
    Ceiling c;
    CeilingConfig cfg = { .hard_limit = 500, .soft_limit = 0 };
    ceiling_init(&c, &cfg);

    ceiling_check(&c, 600);
    ceiling_check(&c, 700);
    ceiling_reset(&c);

    assert(!ceiling_is_active(&c));
    assert(ceiling_breach_count(&c) == 0);
    printf("PASS test_reset\n");
}

static void test_severity_str(void) {
    assert(__builtin_strcmp(ceiling_severity_str(CEILING_OK),   "ok")   == 0);
    assert(__builtin_strcmp(ceiling_severity_str(CEILING_SOFT), "soft") == 0);
    assert(__builtin_strcmp(ceiling_severity_str(CEILING_HARD), "hard") == 0);
    printf("PASS test_severity_str\n");
}

int main(void) {
    test_no_breach();
    test_soft_breach();
    test_hard_breach();
    test_consecutive_hard_breaches();
    test_recovery_resets_consecutive();
    test_reset();
    test_severity_str();
    printf("All ceiling tests passed.\n");
    return 0;
}
