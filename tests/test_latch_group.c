#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/latch.h"
#include "../src/latch_group.h"

static void test_group_init(void) {
    LatchGroup grp;
    latch_group_init(&grp);
    assert(grp.count == 0);
    assert(!grp.any_triggered);
    assert(!grp.all_triggered);
    assert(grp.triggered_count == 0);
    printf("  PASS: group_init\n");
}

static void test_group_add_and_evaluate_none(void) {
    LatchGroup grp;
    Latch a, b;
    latch_group_init(&grp);
    latch_init(&a);
    latch_init(&b);

    assert(latch_group_add(&grp, &a, "latch_a"));
    assert(latch_group_add(&grp, &b, "latch_b"));
    assert(grp.count == 2);

    latch_group_evaluate(&grp);
    assert(!latch_group_any(&grp));
    assert(!latch_group_all(&grp));
    assert(latch_group_triggered_count(&grp) == 0);
    printf("  PASS: group_evaluate_none_triggered\n");
}

static void test_group_any_triggered(void) {
    LatchGroup grp;
    Latch a, b;
    latch_group_init(&grp);
    latch_init(&a);
    latch_init(&b);
    latch_group_add(&grp, &a, "a");
    latch_group_add(&grp, &b, "b");

    latch_set(&a);
    latch_group_evaluate(&grp);
    assert(latch_group_any(&grp));
    assert(!latch_group_all(&grp));
    assert(latch_group_triggered_count(&grp) == 1);
    printf("  PASS: group_any_triggered\n");
}

static void test_group_all_triggered(void) {
    LatchGroup grp;
    Latch a, b, c;
    latch_group_init(&grp);
    latch_init(&a); latch_init(&b); latch_init(&c);
    latch_group_add(&grp, &a, "a");
    latch_group_add(&grp, &b, "b");
    latch_group_add(&grp, &c, "c");

    latch_set(&a); latch_set(&b); latch_set(&c);
    latch_group_evaluate(&grp);
    assert(latch_group_any(&grp));
    assert(latch_group_all(&grp));
    assert(latch_group_triggered_count(&grp) == 3);
    printf("  PASS: group_all_triggered\n");
}

static void test_group_reset_all(void) {
    LatchGroup grp;
    Latch a, b;
    latch_group_init(&grp);
    latch_init(&a); latch_init(&b);
    latch_group_add(&grp, &a, "a");
    latch_group_add(&grp, &b, "b");

    latch_set(&a); latch_set(&b);
    latch_group_evaluate(&grp);
    assert(latch_group_all(&grp));

    latch_group_reset_all(&grp);
    latch_group_evaluate(&grp);
    assert(!latch_group_any(&grp));
    assert(!latch_is_set(&a));
    assert(!latch_is_set(&b));
    printf("  PASS: group_reset_all\n");
}

static void test_group_max_capacity(void) {
    LatchGroup grp;
    latch_group_init(&grp);
    Latch latches[LATCH_GROUP_MAX + 1];
    for (int i = 0; i < LATCH_GROUP_MAX + 1; i++) latch_init(&latches[i]);
    for (int i = 0; i < LATCH_GROUP_MAX; i++) {
        assert(latch_group_add(&grp, &latches[i], "x"));
    }
    assert(!latch_group_add(&grp, &latches[LATCH_GROUP_MAX], "overflow"));
    printf("  PASS: group_max_capacity\n");
}

int main(void) {
    printf("test_latch_group:\n");
    test_group_init();
    test_group_add_and_evaluate_none();
    test_group_any_triggered();
    test_group_all_triggered();
    test_group_reset_all();
    test_group_max_capacity();
    printf("All latch_group tests passed.\n");
    return 0;
}
