#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <stdint.h>
#include <time.h>

typedef struct {
    pid_t       pid;
    struct timespec timestamp;
    uint64_t    vm_rss_kb;
    uint64_t    vm_virt_kb;
    uint64_t    vm_peak_kb;
    uint64_t    vm_swap_kb;
    uint64_t    heap_kb;
    uint64_t    stack_kb;
} mem_snapshot_t;

typedef struct {
    mem_snapshot_t *entries;
    size_t          count;
    size_t          capacity;
} snapshot_log_t;

int  snapshot_log_init(snapshot_log_t *log, size_t initial_capacity);
void snapshot_log_free(snapshot_log_t *log);
int  snapshot_take(pid_t pid, mem_snapshot_t *out);
int  snapshot_log_append(snapshot_log_t *log, const mem_snapshot_t *snap);
void snapshot_print(const mem_snapshot_t *snap);
void snapshot_log_print_all(const snapshot_log_t *log);

#endif /* SNAPSHOT_H */
