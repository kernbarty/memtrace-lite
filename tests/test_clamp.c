#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../src/clamp.h"

static void test_init(void)
{
    clamp_t c;
    clamp_init(&c, 100, 900);
    assert(c.min     == 100);
    assert(c.max     == 900);
    assert(c.clamped == 0);
    assert(c.total   == 0);
    printf("PASS test_init\n");
}

static void test_within_bounds(void)
{
    clamp_t c;
    clamp_init(&c, 0, 1000);
    uint64_t v = clamp_apply(&c, 500);
    assert(v == 500);
    assert(c.total   == 1);
    assert(c.clamped == 0);
    printf("PASS test_within_bounds\n");
}

static void test_below_min(void)
{
    clamp_t c;
    clamp_init(&c, 200, 800);
    uint64_t v = clamp_apply(&c, 50);
    assert(v == 200);
    assert(c.clamped == 1);
    assert(c.total   == 1);
    printf("PASS test_below_min\n");
}

static void test_above_max(void)
{
    clamp_t c;
    clamp_init(&c, 200, 800);
    uint64_t v = clamp_apply(&c, 9999);
    assert(v == 800);
    assert(c.clamped == 1);
    printf("PASS test_above_max\n");
}

static void test_ratio(void)
{
    clamp_t c;
    clamp_init(&c, 100, 500);
    clamp_apply(&c, 100);  /* ok */
    clamp_apply(&c, 600);  /* clamped */
    clamp_apply(&c, 50);   /* clamped */
    clamp_apply(&c, 300);  /* ok */
    double r = clamp_ratio(&c);
    assert(r >= 0.49 && r <= 0.51);  /* 2/4 = 0.5 */
    printf("PASS test_ratio\n");
}

static void test_reset(void)
{
    clamp_t c;
    clamp_init(&c, 0, 100);
    clamp_apply(&c, 200);
    clamp_reset(&c);
    assert(c.clamped == 0);
    assert(c.total   == 0);
    assert(c.min == 0 && c.max == 100); /* bounds preserved */
    printf("PASS test_reset\n");
}

static void test_set_bounds_swap(void)
{
    clamp_t c;
    clamp_init(&c, 0, 0);
    clamp_set_bounds(&c, 900, 100); /* inverted — should swap */
    assert(c.min == 100);
    assert(c.max == 900);
    uint64_t v = clamp_apply(&c, 50);
    assert(v == 100);
    printf("PASS test_set_bounds_swap\n");
}

static void test_null_safety(void)
{
    uint64_t v = clamp_apply(NULL, 42);
    assert(v == 42);
    clamp_reset(NULL);       /* should not crash */
    clamp_set_bounds(NULL, 0, 1); /* should not crash */
    printf("PASS test_null_safety\n");
}

int main(void)
{
    test_init();
    test_within_bounds();
    test_below_min();
    test_above_max();
    test_ratio();
    test_reset();
    test_set_bounds_swap();
    test_null_safety();
    printf("All clamp tests passed.\n");
    return 0;
}
