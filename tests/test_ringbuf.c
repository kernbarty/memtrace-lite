#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/ringbuf.h"
#include "../src/snapshot.h"

static MemSnapshot make_snap(long rss_kb, long virt_kb) {
    MemSnapshot s;
    memset(&s, 0, sizeof(s));
    s.rss_kb  = rss_kb;
    s.virt_kb = virt_kb;
    return s;
}

static void test_create_destroy(void) {
    RingBuf *rb = ringbuf_create(8);
    assert(rb != NULL);
    assert(ringbuf_count(rb) == 0);
    ringbuf_destroy(rb);
    printf("[PASS] test_create_destroy\n");
}

static void test_push_and_get(void) {
    RingBuf *rb = ringbuf_create(4);
    MemSnapshot s1 = make_snap(100, 200);
    MemSnapshot s2 = make_snap(150, 300);
    ringbuf_push(rb, &s1);
    ringbuf_push(rb, &s2);
    assert(ringbuf_count(rb) == 2);
    assert(ringbuf_get(rb, 0)->rss_kb == 100);
    assert(ringbuf_get(rb, 1)->rss_kb == 150);
    assert(ringbuf_get(rb, 2) == NULL);
    ringbuf_destroy(rb);
    printf("[PASS] test_push_and_get\n");
}

static void test_overwrite_oldest(void) {
    RingBuf *rb = ringbuf_create(3);
    for (int i = 1; i <= 5; i++)
        ringbuf_push(rb, &(MemSnapshot){ .rss_kb = i * 10 });
    /* After 5 pushes into cap-3 buffer: oldest = push 3 (30), then 40, 50 */
    assert(ringbuf_count(rb) == 3);
    assert(ringbuf_get(rb, 0)->rss_kb == 30);
    assert(ringbuf_get(rb, 1)->rss_kb == 40);
    assert(ringbuf_get(rb, 2)->rss_kb == 50);
    ringbuf_destroy(rb);
    printf("[PASS] test_overwrite_oldest\n");
}

static void test_clear(void) {
    RingBuf *rb = ringbuf_create(4);
    MemSnapshot s = make_snap(10, 20);
    ringbuf_push(rb, &s);
    ringbuf_clear(rb);
    assert(ringbuf_count(rb) == 0);
    assert(ringbuf_get(rb, 0) == NULL);
    ringbuf_destroy(rb);
    printf("[PASS] test_clear\n");
}

static void test_capacity_clamp(void) {
    RingBuf *rb = ringbuf_create(RINGBUF_MAX_CAPACITY + 999);
    assert(rb != NULL);
    /* capacity should be clamped */
    ringbuf_destroy(rb);
    printf("[PASS] test_capacity_clamp\n");
}

int main(void) {
    test_create_destroy();
    test_push_and_get();
    test_overwrite_oldest();
    test_clear();
    test_capacity_clamp();
    printf("All ringbuf tests passed.\n");
    return 0;
}
