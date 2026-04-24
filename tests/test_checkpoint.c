#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "../src/checkpoint.h"

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK(cond) do { \
    tests_run++; \
    if (cond) { tests_passed++; } \
    else { fprintf(stderr, "FAIL: %s line %d\n", __func__, __LINE__); } \
} while(0)

static void test_init(void) {
    checkpoint_store_t store;
    checkpoint_init(&store);
    CHECK(store.count == 0);
}

static void test_record_and_find(void) {
    checkpoint_store_t store;
    checkpoint_init(&store);

    int r = checkpoint_record(&store, "start", 1024, 4096);
    CHECK(r == 0);
    CHECK(store.count == 1);

    const checkpoint_entry_t *e = checkpoint_find(&store, "start");
    CHECK(e != NULL);
    CHECK(e->rss_kb == 1024);
    CHECK(e->vms_kb == 4096);
    CHECK(strcmp(e->label, "start") == 0);
}

static void test_update_existing(void) {
    checkpoint_store_t store;
    checkpoint_init(&store);

    checkpoint_record(&store, "mark", 500, 2000);
    checkpoint_record(&store, "mark", 600, 2200);
    CHECK(store.count == 1);

    const checkpoint_entry_t *e = checkpoint_find(&store, "mark");
    CHECK(e != NULL);
    CHECK(e->rss_kb == 600);
}

static void test_delta_rss(void) {
    checkpoint_store_t store;
    checkpoint_init(&store);

    checkpoint_record(&store, "before", 2000, 8000);
    checkpoint_record(&store, "after",  2500, 8500);

    int64_t delta = 0;
    int r = checkpoint_delta_rss(&store, "before", "after", &delta);
    CHECK(r == 0);
    CHECK(delta == 500);

    r = checkpoint_delta_rss(&store, "after", "before", &delta);
    CHECK(r == 0);
    CHECK(delta == -500);
}

static void test_delta_missing_label(void) {
    checkpoint_store_t store;
    checkpoint_init(&store);
    checkpoint_record(&store, "only", 100, 400);

    int64_t delta = 0;
    int r = checkpoint_delta_rss(&store, "only", "missing", &delta);
    CHECK(r == -1);
}

static void test_remove(void) {
    checkpoint_store_t store;
    checkpoint_init(&store);

    checkpoint_record(&store, "a", 100, 200);
    checkpoint_record(&store, "b", 300, 600);
    CHECK(store.count == 2);

    int r = checkpoint_remove(&store, "a");
    CHECK(r == 0);
    CHECK(store.count == 1);
    CHECK(checkpoint_find(&store, "a") == NULL);
    CHECK(checkpoint_find(&store, "b") != NULL);

    r = checkpoint_remove(&store, "nonexistent");
    CHECK(r == -1);
}

static void test_reset(void) {
    checkpoint_store_t store;
    checkpoint_init(&store);
    checkpoint_record(&store, "x", 10, 20);
    checkpoint_reset(&store);
    CHECK(store.count == 0);
    CHECK(checkpoint_find(&store, "x") == NULL);
}

static void test_null_safety(void) {
    checkpoint_store_t store;
    checkpoint_init(&store);
    CHECK(checkpoint_record(NULL, "l", 0, 0) == -1);
    CHECK(checkpoint_record(&store, NULL, 0, 0) == -1);
    CHECK(checkpoint_find(NULL, "l") == NULL);
    CHECK(checkpoint_find(&store, NULL) == NULL);
    int64_t d = 0;
    CHECK(checkpoint_delta_rss(NULL, "a", "b", &d) == -1);
    CHECK(checkpoint_remove(NULL, "l") == -1);
}

int main(void) {
    test_init();
    test_record_and_find();
    test_update_existing();
    test_delta_rss();
    test_delta_missing_label();
    test_remove();
    test_reset();
    test_null_safety();

    printf("checkpoint: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
