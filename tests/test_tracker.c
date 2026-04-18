#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "../src/tracker.h"

static void test_init_free(void)
{
    MemTracker t;
    assert(tracker_init(&t, getpid(), 100) == 0);
    assert(t.snapshots != NULL);
    assert(t.count == 0);
    assert(t.capacity == TRACKER_MAX_SNAPSHOTS);
    tracker_free(&t);
    assert(t.snapshots == NULL);
    printf("PASS test_init_free\n");
}

static void test_collect_self(void)
{
    MemTracker t;
    assert(tracker_init(&t, getpid(), 50) == 0);

    int rc = tracker_collect(&t);
    assert(rc == 0);
    assert(t.count == 1);
    assert(t.snapshots[0].rss_kb > 0);
    assert(t.snapshots[0].timestamp_ms >= 0);

    tracker_free(&t);
    printf("PASS test_collect_self\n");
}

static void test_peak_rss(void)
{
    MemTracker t;
    assert(tracker_init(&t, getpid(), 50) == 0);

    for (int i = 0; i < 3; i++)
        assert(tracker_collect(&t) == 0);

    long peak = tracker_peak_rss(&t);
    assert(peak > 0);

    /* peak must be >= every individual sample */
    for (size_t i = 0; i < t.count; i++)
        assert(peak >= t.snapshots[i].rss_kb);

    tracker_free(&t);
    printf("PASS test_peak_rss\n");
}

static void test_peak_rss_empty(void)
{
    MemTracker t;
    assert(tracker_init(&t, getpid(), 50) == 0);
    assert(tracker_peak_rss(&t) == 0);
    tracker_free(&t);
    printf("PASS test_peak_rss_empty\n");
}

int main(void)
{
    test_init_free();
    test_collect_self();
    test_peak_rss();
    test_peak_rss_empty();
    printf("All tracker tests passed.\n");
    return 0;
}
