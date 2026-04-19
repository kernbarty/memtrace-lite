#include <stdio.h>
#include <assert.h>
#include "../src/quota.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond) do { \
    if (cond) { printf("PASS: %s\n", #cond); passed++; } \
    else { printf("FAIL: %s (line %d)\n", #cond, __LINE__); failed++; } \
} while(0)

static void test_init(void) {
    quota_t q;
    quota_init(&q, 512, 1024);
    size_t soft, hard;
    quota_get_limits(&q, &soft, &hard);
    CHECK(soft == 512);
    CHECK(hard == 1024);
    CHECK(q.exceed_count == 0);
    CHECK(q.warn_count == 0);
}

static void test_check_ok(void) {
    quota_t q;
    quota_init(&q, 512, 1024);
    quota_status_t s = quota_check(&q, 256);
    CHECK(s == QUOTA_OK);
    CHECK(q.warn_count == 0);
    CHECK(q.exceed_count == 0);
}

static void test_check_warn(void) {
    quota_t q;
    quota_init(&q, 512, 1024);
    quota_status_t s = quota_check(&q, 600);
    CHECK(s == QUOTA_WARN);
    CHECK(q.warn_count == 1);
    CHECK(q.exceed_count == 0);
}

static void test_check_exceeded(void) {
    quota_t q;
    quota_init(&q, 512, 1024);
    quota_status_t s = quota_check(&q, 1100);
    CHECK(s == QUOTA_EXCEEDED);
    CHECK(q.exceed_count == 1);
}

static void test_reset(void) {
    quota_t q;
    quota_init(&q, 512, 1024);
    quota_check(&q, 600);
    quota_check(&q, 1100);
    quota_reset(&q);
    CHECK(q.warn_count == 0);
    CHECK(q.exceed_count == 0);
    CHECK(q.current_kb == 0);
}

static void test_status_str(void) {
    CHECK(quota_status_str(QUOTA_OK)[0] == 'o');
    CHECK(quota_status_str(QUOTA_WARN)[0] == 'w');
    CHECK(quota_status_str(QUOTA_EXCEEDED)[0] == 'e');
}

static void test_null_safety(void) {
    quota_check(NULL, 100);
    quota_status(NULL);
    quota_get_limits(NULL, NULL, NULL);
    CHECK(1);
}

int main(void) {
    test_init();
    test_check_ok();
    test_check_warn();
    test_check_exceeded();
    test_reset();
    test_status_str();
    test_null_safety();
    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
