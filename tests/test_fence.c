#include <stdio.h>
#include <assert.h>
#include "../src/fence.h"
#include "../src/fence_policy.h"

/* ---- fence_t tests ---- */

static void test_fence_ok(void)
{
    fence_t f;
    fence_init(&f, 512, 1024);
    assert(fence_check(&f, 256) == FENCE_OK);
    assert(f.warn_count   == 0);
    assert(f.breach_count == 0);
    assert(f.peak_kb      == 256);
    printf("PASS test_fence_ok\n");
}

static void test_fence_warn(void)
{
    fence_t f;
    fence_init(&f, 512, 1024);
    assert(fence_check(&f, 512) == FENCE_WARN);
    assert(fence_check(&f, 600) == FENCE_WARN);
    assert(f.warn_count   == 2);
    assert(f.breach_count == 0);
    assert(f.peak_kb      == 600);
    printf("PASS test_fence_warn\n");
}

static void test_fence_breach(void)
{
    fence_t f;
    fence_init(&f, 512, 1024);
    fence_check(&f, 512);   /* warn */
    fence_check(&f, 1024);  /* breach */
    assert(f.breach_count == 1);
    assert(f.warn_count   == 1);
    printf("PASS test_fence_breach\n");
}

static void test_fence_peak(void)
{
    fence_t f;
    fence_init(&f, 512, 1024);
    fence_check(&f, 300);
    fence_check(&f, 700);
    fence_check(&f, 500);
    assert(f.peak_kb == 700);
    printf("PASS test_fence_peak\n");
}

static void test_fence_reset(void)
{
    fence_t f;
    fence_init(&f, 512, 1024);
    fence_check(&f, 600);
    fence_check(&f, 1100);
    fence_reset(&f);
    assert(f.warn_count   == 0);
    assert(f.breach_count == 0);
    assert(f.peak_kb      == 0);
    assert(f.soft_kb      == 512);  /* limits preserved */
    assert(f.hard_kb      == 1024);
    printf("PASS test_fence_reset\n");
}

static void test_fence_status_str(void)
{
    assert(fence_status_str(FENCE_OK)     != NULL);
    assert(fence_status_str(FENCE_WARN)   != NULL);
    assert(fence_status_str(FENCE_BREACH) != NULL);
    printf("PASS test_fence_status_str\n");
}

/* ---- fence_policy_t tests ---- */

static int g_warn_fired    = 0;
static int g_breach_fired  = 0;
static size_t g_last_kb    = 0;

static void on_warn(fence_status_t s, size_t kb, void *ud)
{
    (void)s; (void)ud;
    g_warn_fired++;
    g_last_kb = kb;
}

static void on_breach(fence_status_t s, size_t kb, void *ud)
{
    (void)s; (void)ud;
    g_breach_fired++;
    g_last_kb = kb;
}

static void test_policy_callbacks(void)
{
    fence_t f;
    fence_policy_t p;
    fence_init(&f, 512, 1024);
    fence_policy_init(&p, &f, on_warn, on_breach, NULL);

    g_warn_fired = g_breach_fired = 0;

    fence_policy_check(&p, 256);   /* ok   — no callback */
    assert(g_warn_fired == 0 && g_breach_fired == 0);

    fence_policy_check(&p, 600);   /* warn */
    assert(g_warn_fired == 1 && g_breach_fired == 0);
    assert(g_last_kb == 600);

    fence_policy_check(&p, 1100);  /* breach */
    assert(g_breach_fired == 1);
    assert(g_last_kb == 1100);

    printf("PASS test_policy_callbacks\n");
}

int main(void)
{
    test_fence_ok();
    test_fence_warn();
    test_fence_breach();
    test_fence_peak();
    test_fence_reset();
    test_fence_status_str();
    test_policy_callbacks();
    printf("All fence tests passed.\n");
    return 0;
}
