#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "../src/history.h"
#include "../src/snapshot.h"

static MemSnapshot make_snap(unsigned long rss, unsigned long virt) {
    MemSnapshot s;
    memset(&s, 0, sizeof(s));
    s.rss_kb  = rss;
    s.virt_kb = virt;
    return s;
}

static void test_init(void) {
    MemHistory h;
    history_init(&h);
    assert(history_count(&h) == 0);
    assert(history_peak_rss(&h) == 0);
    printf("PASS test_init\n");
}

static void test_push_and_count(void) {
    MemHistory h;
    history_init(&h);
    MemSnapshot s = make_snap(1024, 4096);
    history_push(&h, &s, time(NULL));
    assert(history_count(&h) == 1);
    history_push(&h, &s, time(NULL));
    assert(history_count(&h) == 2);
    printf("PASS test_push_and_count\n");
}

static void test_get_order(void) {
    MemHistory h;
    history_init(&h);
    time_t base = 1000;
    for (int i = 0; i < 5; i++) {
        MemSnapshot s = make_snap((unsigned long)(i + 1) * 100, 0);
        history_push(&h, &s, base + i);
    }
    HistoryEntry out[5];
    size_t n = history_get(&h, out, 5);
    assert(n == 5);
    for (size_t i = 0; i < n; i++) {
        assert(out[i].timestamp == base + (time_t)i);
        assert(out[i].snapshot.rss_kb == (unsigned long)(i + 1) * 100);
    }
    printf("PASS test_get_order\n");
}

static void test_peak_rss(void) {
    MemHistory h;
    history_init(&h);
    unsigned long vals[] = {512, 2048, 1024, 4096, 256};
    for (int i = 0; i < 5; i++) {
        MemSnapshot s = make_snap(vals[i], 0);
        history_push(&h, &s, (time_t)(1000 + i));
    }
    assert(history_peak_rss(&h) == 4096);
    printf("PASS test_peak_rss\n");
}

static void test_circular_wrap(void) {
    MemHistory h;
    history_init(&h);
    /* Fill beyond HISTORY_MAX_ENTRIES */
    for (int i = 0; i < HISTORY_MAX_ENTRIES + 10; i++) {
        MemSnapshot s = make_snap((unsigned long)i, 0);
        history_push(&h, &s, (time_t)i);
    }
    assert(history_count(&h) == HISTORY_MAX_ENTRIES);
    HistoryEntry out[HISTORY_MAX_ENTRIES];
    size_t n = history_get(&h, out, HISTORY_MAX_ENTRIES);
    assert(n == HISTORY_MAX_ENTRIES);
    /* Oldest surviving entry should be index 10 */
    assert(out[0].snapshot.rss_kb == 10);
    printf("PASS test_circular_wrap\n");
}

int main(void) {
    test_init();
    test_push_and_count();
    test_get_order();
    test_peak_rss();
    test_circular_wrap();
    printf("All history tests passed.\n");
    return 0;
}
