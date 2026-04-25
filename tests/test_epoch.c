#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "../src/epoch.h"

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; printf("  PASS: %s\n", msg); } \
    else { printf("  FAIL: %s\n", msg); } \
} while(0)

static void test_open_close(void) {
    Epoch e;
    epoch_open(&e, 1);
    CHECK(e.seq == 1, "seq set on open");
    CHECK(epoch_is_open(&e), "epoch open after epoch_open");
    CHECK(e.sample_count == 0, "sample_count zero on open");
    CHECK(e.bytes_total  == 0, "bytes_total zero on open");

    epoch_close(&e);
    CHECK(!epoch_is_open(&e), "epoch closed after epoch_close");
}

static void test_record(void) {
    Epoch e;
    epoch_open(&e, 2);
    epoch_record(&e, 1024);
    epoch_record(&e, 2048);
    CHECK(e.sample_count == 2,    "sample_count after two records");
    CHECK(e.bytes_total  == 3072, "bytes_total after two records");
}

static void test_elapsed_open(void) {
    Epoch e;
    epoch_open(&e, 3);
    usleep(15000); /* ~15 ms */
    uint64_t ms = epoch_elapsed_ms(&e);
    CHECK(ms >= 10, "elapsed >= 10 ms while open");
    CHECK(ms < 500, "elapsed < 500 ms while open (sanity)");
}

static void test_elapsed_closed(void) {
    Epoch e;
    epoch_open(&e, 4);
    usleep(20000); /* ~20 ms */
    epoch_close(&e);
    uint64_t ms_at_close = epoch_elapsed_ms(&e);
    usleep(30000); /* wait more — should not affect result */
    uint64_t ms_later = epoch_elapsed_ms(&e);
    CHECK(ms_at_close >= 10,          "elapsed >= 10 ms after close");
    CHECK(ms_at_close == ms_later,    "elapsed frozen after close");
}

static void test_reset(void) {
    Epoch e;
    epoch_open(&e, 5);
    epoch_record(&e, 512);
    epoch_close(&e);
    epoch_reset(&e);
    CHECK(e.seq          == 0, "seq zero after reset");
    CHECK(e.sample_count == 0, "sample_count zero after reset");
    CHECK(e.bytes_total  == 0, "bytes_total zero after reset");
    CHECK(epoch_is_open(&e),   "epoch open (zero) after reset");
}

static void test_seq_monotonic(void) {
    Epoch a, b;
    epoch_open(&a, 10);
    epoch_open(&b, 11);
    CHECK(b.seq > a.seq, "seq monotonically increases");
}

int main(void) {
    printf("=== test_epoch ===\n");
    test_open_close();
    test_record();
    test_elapsed_open();
    test_elapsed_closed();
    test_reset();
    test_seq_monotonic();
    printf("%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
