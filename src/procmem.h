#ifndef PROCMEM_H
#define PROCMEM_H

#include <stdint.h>
#include <sys/types.h>

/* Memory snapshot for a single process */
typedef struct {
    pid_t   pid;
    uint64_t vm_rss_kb;   /* Resident Set Size in KB */
    uint64_t vm_virt_kb;  /* Virtual memory size in KB */
    uint64_t vm_swap_kb;  /* Swap usage in KB */
} procmem_snapshot_t;

/**
 * Read memory stats for the given pid from /proc/<pid>/status.
 * Returns 0 on success, -1 on error.
 */
int procmem_read(pid_t pid, procmem_snapshot_t *snap);

/**
 * Print a snapshot to stdout in human-readable form.
 */
void procmem_print(const procmem_snapshot_t *snap);

#endif /* PROCMEM_H */
