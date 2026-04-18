#include <stdio.h>
#include <assert.h>
#include "../src/ringbuf.h"
#include "../src/snapshot.h"

static MemSnapshot make_snap(long rss_kb) {
    MemSnapshot s = {0};
    s.rss_kb = rss_kb;
    s.timestamp = (time_t)rss_kb;
    return s;
}

void test_create_destroy() {
    RingBuf *rb = ringbuf_create(4);
    assert(rb != NULL);
    assert(rb->capacity == 4);
    assert(ringbuf_is_empty(rb));
    ringbuf_destroy(rb);
    printf("PASS test_create_destroy\n");
}

void test_push_pop() {
    RingBuf *rb = ringbuf_create(3);
    MemSnapshot s = make_snap(100);
    assert(ringbuf_push(rb, &s));
    assert(rb->count == 1);
    MemSnapshot out;
    assert(ringbuf_pop(rb, &out));
    assert(out.rss_kb == 100);
    assert(ringbuf_is_empty(rb));
    ringbuf_destroy(rb);
    printf("PASS test_push_pop\n");
}

void test_overflow_overwrites() {
    RingBuf *rb = ringbuf_create(3);
    for (int i = 1; i <= 5; i++) {
        MemSnapshot s = make_snap(i * 10);
        ringbuf_push(rb, &s);
    }
    assert(rb->count == 3);
    MemSnapshot out;
    ringbuf_pop(rb, &out); assert(out.rss_kb == 30);
    ringbuf_pop(rb, &out); assert(out.rss_kb == 40);
    ringbuf_pop(rb, &out); assert(out.rss_kb == 50);
    assert(ringbuf_is_empty(rb));
    ringbuf_destroy(rb);
    printf("PASS test_overflow_overwrites\n");
}

void test_peek() {
    RingBuf *rb = ringbuf_create(4);
    for (int i = 1; i <= 3; i++) {
        MemSnapshot s = make_snap(i * 5);
        ringbuf_push(rb, &s);
    }
    MemSnapshot out;
    assert(ringbuf_peek(rb, 0, &out) && out.rss_kb == 5);
    assert(ringbuf_peek(rb, 2, &out) && out.rss_kb == 15);
    assert(!ringbuf_peek(rb, 3, &out));
    ringbuf_destroy(rb);
    printf("PASS test_peek\n");
}

void test_clear() {
    RingBuf *rb = ringbuf_create(4);
    MemSnapshot s = make_snap(99);
    ringbuf_push(rb, &s);
    ringbuf_clear(rb);
    assert(ringbuf_is_empty(rb));
    ringbuf_destroy(rb);
    printf("PASS test_clear\n");
}

int main() {
    test_create_destroy();
    test_push_pop();
    test_overflow_overwrites();
    test_peek();
    test_clear();
    printf("All ringbuf tests passed.\n");
    return 0;
}
