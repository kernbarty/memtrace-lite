#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "../src/procmem.h"

static void test_read_self(void) {
    procmem_snapshot_t snap;
    pid_t self = getpid();

    int rc = procmem_read(self, &snap);
    assert(rc == 0 && "procmem_read should succeed for self");
    assert(snap.pid == self);
    assert(snap.vm_rss_kb > 0  && "RSS must be non-zero for a live process");
    assert(snap.vm_virt_kb > 0 && "Virtual size must be non-zero for a live process");

    printf("[PASS] test_read_self\n");
    procmem_print(&snap);
}

static void test_read_invalid_pid(void) {
    procmem_snapshot_t snap;
    /* PID 0 is not a valid user process; /proc/0/status should not exist */
    int rc = procmem_read(0, &snap);
    assert(rc == -1 && "procmem_read should fail for pid 0");
    printf("[PASS] test_read_invalid_pid\n");
}

int main(void) {
    printf("=== procmem tests ===\n");
    test_read_self();
    test_read_invalid_pid();
    printf("All tests passed.\n");
    return EXIT_SUCCESS;
}
