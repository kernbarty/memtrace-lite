#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "snapshot.h"
#include "procmem.h"

int snapshot_log_init(snapshot_log_t *log, size_t initial_capacity) {
    if (!log || initial_capacity == 0) return -1;
    log->entries = malloc(sizeof(mem_snapshot_t) * initial_capacity);
    if (!log->entries) return -1;
    log->count    = 0;
    log->capacity = initial_capacity;
    return 0;
}

void snapshot_log_free(snapshot_log_t *log) {
    if (!log) return;
    free(log->entries);
    log->entries  = NULL;
    log->count    = 0;
    log->capacity = 0;
}

int snapshot_take(pid_t pid, mem_snapshot_t *out) {
    if (!out) return -1;

    proc_mem_info_t info;
    if (procmem_read(pid, &info) != 0) return -1;

    memset(out, 0, sizeof(*out));
    out->pid       = pid;
    out->vm_rss_kb = info.vm_rss_kb;
    out->vm_virt_kb= info.vm_size_kb;
    out->vm_peak_kb= info.vm_peak_kb;
    out->vm_swap_kb= info.vm_swap_kb;
    out->heap_kb   = info.heap_kb;
    out->stack_kb  = info.stack_kb;
    clock_gettime(CLOCK_MONOTONIC, &out->timestamp);
    return 0;
}

int snapshot_log_append(snapshot_log_t *log, const mem_snapshot_t *snap) {
    if (!log || !snap) return -1;
    if (log->count >= log->capacity) {
        size_t new_cap = log->capacity * 2;
        mem_snapshot_t *tmp = realloc(log->entries, sizeof(mem_snapshot_t) * new_cap);
        if (!tmp) return -1;
        log->entries  = tmp;
        log->capacity = new_cap;
    }
    log->entries[log->count++] = *snap;
    return 0;
}

void snapshot_print(const mem_snapshot_t *snap) {
    if (!snap) return;
    printf("[%ld.%03ld] pid=%-6d rss=%6lu kb  virt=%8lu kb  peak=%6lu kb  swap=%6lu kb  heap=%6lu kb  stack=%6lu kb\n",
           snap->timestamp.tv_sec,
           snap->timestamp.tv_nsec / 1000000L,
           snap->pid,
           snap->vm_rss_kb, snap->vm_virt_kb, snap->vm_peak_kb,
           snap->vm_swap_kb, snap->heap_kb, snap->stack_kb);
}

void snapshot_log_print_all(const snapshot_log_t *log) {
    if (!log) return;
    for (size_t i = 0; i < log->count; i++)
        snapshot_print(&log->entries[i]);
}
