#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../src/ceiling.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  [TEST] %s\n", name); } while(0)
#define PASS() do { tests_passed++; } while(0)
#define FAIL(msg) do { fprintf(stderr, "  [FAIL] %s\n", msg); } while(0)

static void test_init(void) {
    TEST("ceiling_init sets limit and clears state");
    Ceiling c;
    ceiling_init(&c, 1024);
    assert(c.limit == 1024);
    assert(c.violated == 0);
    assert(c.violation_count == 0);
    assert(c.last_value == 0);
    PASS();
}

static void test_eval_under_limit(void) {
    TEST("ceiling_eval returns value when under limit");
    Ceiling c;
    ceiling_init(&c, 1000);
    size_t result = ceiling_eval(&c, 500);
    assert(result == 500);
    assert(c.violated == 0);
    assert(c.violation_count == 0);
    PASS();
}

static void test_eval_at_limit(void) {
    TEST("ceiling_eval returns limit when value equals limit");
    Ceiling c;
    ceiling_init(&c, 1000);
    size_t result = ceiling_eval(&c, 1000);
    assert(result == 1000);
    assert(c.violated == 0);
    assert(c.violation_count == 0);
    PASS();
}

static void test_eval_over_limit(void) {
    TEST("ceiling_eval clamps and records violation when over limit");
    Ceiling c;
    ceiling_init(&c, 1000);
    size_t result = ceiling_eval(&c, 1500);
    assert(result == 1000);
    assert(ceiling_is_violated(&c) != 0);
    assert(ceiling_violation_count(&c) == 1);
    assert(c.last_value == 1500);
    PASS();
}

static void test_multiple_violations(void) {
    TEST("ceiling_eval accumulates violation count");
    Ceiling c;
    ceiling_init(&c, 512);
    ceiling_eval(&c, 600);
    ceiling_eval(&c, 700);
    ceiling_eval(&c, 400); /* no violation */
    ceiling_eval(&c, 900);
    assert(ceiling_violation_count(&c) == 3);
    assert(ceiling_is_violated(&c) != 0);
    PASS();
}

static void test_reset(void) {
    TEST("ceiling_reset clears violation state but preserves limit");
    Ceiling c;
    ceiling_init(&c, 256);
    ceiling_eval(&c, 512);
    assert(ceiling_violation_count(&c) == 1);
    ceiling_reset(&c);
    assert(c.violation_count == 0);
    assert(c.violated == 0);
    assert(c.limit == 256);
    PASS();
}

static void test_set_limit(void) {
    TEST("ceiling_set_limit updates limit and resets state");
    Ceiling c;
    ceiling_init(&c, 100);
    ceiling_eval(&c, 200);
    assert(ceiling_violation_count(&c) == 1);
    ceiling_set_limit(&c, 500);
    assert(c.limit == 500);
    assert(c.violation_count == 0);
    assert(c.violated == 0);
    /* value previously over old limit is now fine */
    size_t result = ceiling_eval(&c, 200);
    assert(result == 200);
    assert(ceiling_is_violated(&c) == 0);
    PASS();
}

int main(void) {
    printf("=== test_ceiling ===\n");
    test_init();
    test_eval_under_limit();
    test_eval_at_limit();
    test_eval_over_limit();
    test_multiple_violations();
    test_reset();
    test_set_limit();
    printf("Results: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
