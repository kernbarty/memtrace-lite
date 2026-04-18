#include <stdio.h>
#include <assert.h>
#include "../src/history.h"
#include "../src/history_query.h"

static MemSnapshot make_snap(long rss, long vms) {
    MemSnapshot s = {0};
    s.rss_kb = rss;
    s.vms_kb = vms;
    return s;
}

void test_history_basic(void) {
    History h;
    assert(history_init(&h, 4));
    assert(history_count(&h) == 0);
    MemSnapshot s = make_snap(100, 200);
    assert(history_push(&h, &s));
    assert(history_count(&h) == 1);
    MemSnapshot out;
    assert(history_latest(&h, &out));
    assert(out.rss_kb == 100);
    history_free(&h);
    printf("PASS test_history_basic\n");
}

void test_history_wrap(void) {
    History h;
    assert(history_init(&h, 3));
    for (int i = 1; i <= 5; i++) {
        MemSnapshot s = make_snap(i * 10, i * 20);
        history_push(&h, &s);
    }
    assert(history_count(&h) == 3);
    MemSnapshot out;
    history_get(&h, 0, &out);
    assert(out.rss_kb == 30);
    history_get(&h, 2, &out);
    assert(out.rss_kb == 50);
    history_free(&h);
    printf("PASS test_history_wrap\n");
}

void test_history_query_stats(void) {
    History h;
    assert(history_init(&h, 8));
    history_push(&h, &(MemSnapshot){.rss_kb=100,.vms_kb=200});
    history_push(&h, &(MemSnapshot){.rss_kb=200,.vms_kb=400});
    history_push(&h, &(MemSnapshot){.rss_kb=150,.vms_kb=300});
    HistoryStats st;
    assert(history_query_stats(&h, &st));
    assert(st.sample_count == 3);
    assert(st.rss_peak_kb == 200);
    assert(st.rss_delta_kb == 50);
    history_free(&h);
    printf("PASS test_history_query_stats\n");
}

void test_history_query_peak(void) {
    History h;
    assert(history_init(&h, 4));
    history_push(&h, &(MemSnapshot){.rss_kb=50});
    history_push(&h, &(MemSnapshot){.rss_kb=300});
    history_push(&h, &(MemSnapshot){.rss_kb=100});
    MemSnapshot peak;
    assert(history_query_peak_rss(&h, &peak));
    assert(peak.rss_kb == 300);
    history_free(&h);
    printf("PASS test_history_query_peak\n");
}

void test_history_query_find_exceed(void) {
    History h;
    assert(history_init(&h, 8));
    history_push(&h, &(MemSnapshot){.rss_kb=50});
    history_push(&h, &(MemSnapshot){.rss_kb=80});
    history_push(&h, &(MemSnapshot){.rss_kb=200});
    size_t idx;
    assert(history_query_find_rss_exceed(&h, 100, &idx));
    assert(idx == 2);
    assert(!history_query_find_rss_exceed(&h, 500, &idx));
    history_free(&h);
    printf("PASS test_history_query_find_exceed\n");
}

int main(void) {
    test_history_basic();
    test_history_wrap();
    test_history_query_stats();
    test_history_query_peak();
    test_history_query_find_exceed();
    printf("All history tests passed.\n");
    return 0;
}
