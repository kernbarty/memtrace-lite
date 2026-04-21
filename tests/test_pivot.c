#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "../src/pivot.h"

static void test_init_invalid(void)
{
    Pivot p;
    assert(pivot_init(NULL, 4) == -1);
    assert(pivot_init(&p, 1)   == -1);  /* capacity < 2 */
    printf("[PASS] test_init_invalid\n");
}

static void test_basic_pivot(void)
{
    Pivot p;
    assert(pivot_init(&p, 4) == 0);

    pivot_update(&p, 10.0);
    pivot_update(&p, 20.0);

    /* min=10, max=20 -> pivot=15, upper=20, lower=10 */
    assert(fabs(p.pivot - 15.0) < 1e-9);
    assert(fabs(p.upper - 20.0) < 1e-9);
    assert(fabs(p.lower - 10.0) < 1e-9);

    pivot_free(&p);
    printf("[PASS] test_basic_pivot\n");
}

static void test_classify(void)
{
    Pivot p;
    assert(pivot_init(&p, 6) == 0);

    /* Only one sample: classify should return 0 */
    pivot_update(&p, 50.0);
    assert(pivot_classify(&p, 100.0) == 0);

    pivot_update(&p, 100.0);
    /* pivot=75, upper=100, lower=50 */
    assert(pivot_classify(&p, 101.0) ==  1);
    assert(pivot_classify(&p,  49.0) == -1);
    assert(pivot_classify(&p,  75.0) ==  0);

    pivot_free(&p);
    printf("[PASS] test_classify\n");
}

static void test_window_wrap(void)
{
    Pivot p;
    assert(pivot_init(&p, 3) == 0);

    pivot_update(&p, 10.0);
    pivot_update(&p, 20.0);
    pivot_update(&p, 30.0);
    /* window full: min=10, max=30, pivot=20 */
    assert(fabs(p.pivot - 20.0) < 1e-9);

    /* Push 40, evicts 10: min=20, max=40, pivot=30 */
    pivot_update(&p, 40.0);
    assert(fabs(p.pivot - 30.0) < 1e-9);

    pivot_free(&p);
    printf("[PASS] test_window_wrap\n");
}

static void test_reset(void)
{
    Pivot p;
    assert(pivot_init(&p, 4) == 0);
    pivot_update(&p, 5.0);
    pivot_update(&p, 15.0);
    pivot_reset(&p);
    assert(p.count == 0);
    assert(pivot_classify(&p, 999.0) == 0);
    pivot_free(&p);
    printf("[PASS] test_reset\n");
}

int main(void)
{
    test_init_invalid();
    test_basic_pivot();
    test_classify();
    test_window_wrap();
    test_reset();
    printf("All pivot tests passed.\n");
    return 0;
}
