#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../src/jitter.h"

static void test_jitter_none(void) {
    jitter_t j;
    jitter_init(&j, JITTER_NONE, 10, 1000, 42);
    for (int i = 0; i < 20; i++)
        assert(jitter_apply(&j, 500) == 500);
    printf("PASS test_jitter_none\n");
}

static void test_jitter_full(void) {
    jitter_t j;
    jitter_init(&j, JITTER_FULL, 0, 1000, 1);
    for (int i = 0; i < 50; i++) {
        uint32_t v = jitter_apply(&j, 200);
        assert(v <= 200);
    }
    printf("PASS test_jitter_full\n");
}

static void test_jitter_equal(void) {
    jitter_t j;
    jitter_init(&j, JITTER_EQUAL, 0, 1000, 7);
    for (int i = 0; i < 50; i++) {
        uint32_t v = jitter_apply(&j, 100);
        assert(v >= 50 && v <= 100);
    }
    printf("PASS test_jitter_equal\n");
}

static void test_jitter_decorr(void) {
    jitter_t j;
    jitter_init(&j, JITTER_DECORR, 10, 500, 99);
    for (int i = 0; i < 50; i++) {
        uint32_t v = jitter_apply(&j, 100);
        assert(v <= 500);
        assert(v >= 10);
    }
    printf("PASS test_jitter_decorr\n");
}

static void test_jitter_reset(void) {
    jitter_t j;
    jitter_init(&j, JITTER_FULL, 0, 1000, 55);
    uint32_t a = jitter_apply(&j, 300);
    jitter_reset(&j);
    uint32_t b = jitter_apply(&j, 300);
    /* After reset seed is same default, so results should match */
    assert(a == b);
    printf("PASS test_jitter_reset\n");
}

static void test_null_safety(void) {
    assert(jitter_apply(NULL, 100) == 100);
    jitter_init(NULL, JITTER_NONE, 0, 0, 0);
    jitter_reset(NULL);
    printf("PASS test_null_safety\n");
}

int main(void) {
    test_jitter_none();
    test_jitter_full();
    test_jitter_equal();
    test_jitter_decorr();
    test_jitter_reset();
    test_null_safety();
    printf("All jitter tests passed.\n");
    return 0;
}
