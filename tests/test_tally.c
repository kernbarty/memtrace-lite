#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../src/tally.h"

static void test_init(void)
{
    Tally t;
    tally_init(&t);
    assert(t.num_categories == 0);
    assert(t.generation == 0);
    printf("  [PASS] test_init\n");
}

static void test_register_and_add(void)
{
    Tally t;
    tally_init(&t);

    int a = tally_register(&t, "alloc");
    int f = tally_register(&t, "free");
    assert(a == 0);
    assert(f == 1);

    assert(tally_add(&t, a, 1024) == 0);
    assert(tally_add(&t, a, 2048) == 0);
    assert(tally_inc(&t, f)      == 0);

    const TallyCategory *ca = tally_get(&t, "alloc");
    const TallyCategory *cf = tally_get(&t, "free");

    assert(ca != NULL && ca->count == 2 && ca->total == 3072);
    assert(cf != NULL && cf->count == 1 && cf->total == 0);
    printf("  [PASS] test_register_and_add\n");
}

static void test_duplicate_register(void)
{
    Tally t;
    tally_init(&t);

    int first  = tally_register(&t, "alloc");
    int second = tally_register(&t, "alloc");
    assert(first == second);
    assert(t.num_categories == 1);
    printf("  [PASS] test_duplicate_register\n");
}

static void test_invalid_index(void)
{
    Tally t;
    tally_init(&t);
    assert(tally_add(&t, -1, 100) == -1);
    assert(tally_add(&t,  0, 100) == -1);
    printf("  [PASS] test_invalid_index\n");
}

static void test_reset(void)
{
    Tally t;
    tally_init(&t);
    int idx = tally_register(&t, "overflow");
    tally_add(&t, idx, 512);
    tally_add(&t, idx, 256);

    tally_reset(&t);
    assert(t.generation == 1);
    const TallyCategory *c = tally_get(&t, "overflow");
    assert(c->count == 0 && c->total == 0);
    /* category still registered after reset */
    assert(t.num_categories == 1);
    printf("  [PASS] test_reset\n");
}

static void test_format(void)
{
    Tally t;
    tally_init(&t);
    int idx = tally_register(&t, "pressure");
    tally_add(&t, idx, 4096);

    char buf[512];
    int n = tally_format(&t, buf, sizeof(buf));
    assert(n > 0);
    assert(strstr(buf, "pressure") != NULL);
    assert(strstr(buf, "count=")   != NULL);
    printf("  [PASS] test_format\n");
}

static void test_max_categories(void)
{
    Tally t;
    tally_init(&t);
    char name[TALLY_NAME_LEN];
    for (int i = 0; i < TALLY_MAX_CATEGORIES; i++) {
        snprintf(name, sizeof(name), "cat%d", i);
        assert(tally_register(&t, name) == i);
    }
    /* One over the limit */
    assert(tally_register(&t, "overflow_cat") == -1);
    printf("  [PASS] test_max_categories\n");
}

int main(void)
{
    printf("Running tally tests...\n");
    test_init();
    test_register_and_add();
    test_duplicate_register();
    test_invalid_index();
    test_reset();
    test_format();
    test_max_categories();
    printf("All tally tests passed.\n");
    return 0;
}
