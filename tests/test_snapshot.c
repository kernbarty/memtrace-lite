#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "../src/snapshot.h"
#include "../src/sampler.h"

static void test_snapshot_self(void) {
    mem_snapshot_t snap;
    pid_t self = getpid();
    int rc = snapshot_take(self, &snap);
    assert(rc == 0);
    assert(snap.pid == self);
    assert(snap.vm_rss_kb > 0);
    assert(snap.vm_virt_kb > 0);
    printf("[PASS] snapshot_take(self)\n");
    snapshot_print(&snap);
}

static void test_snapshot_log(void) {
    snapshot_log_t log;
    assert(snapshot_log_init(&log, 4) == 0);

    pid_t self = getpid();
    for (int i = 0; i < 6; i++) {   /* intentionally exceed initial capacity */
        mem_snapshot_t snap;
        assert(snapshot_take(self, &snap) == 0);
        assert(snapshot_log_append(&log, &snap) == 0);
    }
    assert(log.count == 6);
    assert(log.capacity >= 6);
    printf("[PASS] snapshot_log growth (%zu entries)\n", log.count);
    snapshot_log_free(&log);
}

static void test_sampler_limited(void) {
    snapshot_log_t log;
    assert(snapshot_log_init(&log, 8) == 0);

    sampler_config_t cfg = {
        .pid         = getpid(),
        .interval_ms = 10,
        .max_samples = 3,
        .log         = &log,
    };
    int n = sampler_run(&cfg);
    assert(n == 3);
    assert(log.count == 3);
    printf("[PASS] sampler_run collected %d samples\n", n);
    snapshot_log_free(&log);
}

int main(void) {
    test_snapshot_self();
    test_snapshot_log();
    test_sampler_limited();
    printf("All snapshot/sampler tests passed.\n");
    return 0;
}
