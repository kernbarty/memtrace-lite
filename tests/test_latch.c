#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/latch.h"
#include "../src/latch_group.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond) do { \
    if (cond) { printf("  PASS: %s\n", #cond); passed++; } \
    else      { printf("  FAIL: %s  (line %d)\n", #cond, __LINE__); failed++; } \
} while (0)

static void test_latch_basic(void)
{
    puts("[latch] basic arm/trigger/reset");
    latch_t l;
    latch_init(&l, "rss_high");

    CHECK(l.state == LATCH_ARMED);
    CHECK(!latch_is_triggered(&l));

    /* First trigger should succeed */
    CHECK(latch_trigger(&l, 512000));
    CHECK(latch_is_triggered(&l));
    CHECK(l.trigger_value == 512000);
    CHECK(l.trigger_count == 1);

    /* Second trigger must be ignored */
    CHECK(!latch_trigger(&l, 999999));
    CHECK(l.trigger_value == 512000);  /* unchanged */
    CHECK(l.trigger_count == 1);

    /* Reset and re-trigger */
    latch_reset(&l);
    CHECK(!latch_is_triggered(&l));
    CHECK(latch_trigger(&l, 1024000));
    CHECK(l.trigger_count == 2);
}

static void test_latch_disarm(void)
{
    puts("[latch] disarm prevents trigger");
    latch_t l;
    latch_init(&l, "swap");
    latch_disarm(&l);

    CHECK(l.state == LATCH_DISARMED);
    CHECK(!latch_trigger(&l, 100));
    CHECK(!latch_is_triggered(&l));

    latch_arm(&l);
    CHECK(latch_trigger(&l, 100));
    CHECK(latch_is_triggered(&l));
}

static void test_latch_group(void)
{
    puts("[latch_group] add / trigger / count / reset");
    latch_group_t g;
    latch_group_init(&g);

    int ia = latch_group_add(&g, "rss");
    int ib = latch_group_add(&g, "vms");
    int ic = latch_group_add(&g, "swap");
    CHECK(ia == 0 && ib == 1 && ic == 2);
    CHECK(g.count == 3);

    CHECK(latch_group_triggered_count(&g) == 0);

    CHECK(latch_group_trigger(&g, "rss",  400000));
    CHECK(latch_group_trigger(&g, "swap",  8000));
    CHECK(!latch_group_trigger(&g, "rss", 500000));  /* already latched */

    CHECK(latch_group_triggered_count(&g) == 2);

    latch_t *vms = latch_group_find(&g, "vms");
    CHECK(vms != NULL);
    CHECK(!latch_is_triggered(vms));

    latch_group_reset_all(&g);
    CHECK(latch_group_triggered_count(&g) == 0);

    /* unknown label should not crash */
    CHECK(!latch_group_trigger(&g, "unknown", 1));
}

int main(void)
{
    test_latch_basic();
    test_latch_disarm();
    test_latch_group();

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
