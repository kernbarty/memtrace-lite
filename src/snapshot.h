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

/**
 * Initialize a snapshot log with the given initial capacity.
 * Returns 0 on success, -1 on allocation failure.
 */
int  snapshot_log_init(snapshot_log_t *log, size_t initial_capacity);

/**
 * Free all resources associated with a snapshot log.
 */
void snapshot_log_free(snapshot_log_t *log);

/**
 * Read current memory statistics for the given pid into out.
 * Returns 0 on success, -1 if /proc/<pid>/status could not be read.
 */
int  snapshot_take(pid_t pid, mem_snapshot_t *out);

/**
 * Append a copy of snap to the log, growing the buffer if needed.
 * Returns 0 on success, -1 on allocation failure.
 */
int  snapshot_log_append(snapshot_log_t *log, const mem_snapshot_t *snap);

/**
 * Print a single snapshot to stdout in a human-readable format.
 */
void snapshot_print(const mem_snapshot_t *snap);

/**
 * Print all snapshots in the log to stdout.
 */
void snapshot_log_print_all(const snapshot_log_t *log);

/**
 * Compute the delta between two snapshots (later - earlier) and store the
 * result in out.  The pid and timestamp of 'later' are preserved in out.
 * Useful for reporting memory growth between two points in time.
 */
void snapshot_diff(const mem_snapshot_t *earlier,
                   const mem_snapshot_t *later,
                   mem_snapshot_t       *out);

#endif /* SNAPSHOT_H */
