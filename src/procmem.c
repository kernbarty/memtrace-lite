#include "procmem.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define STATUS_PATH_FMT "/proc/%d/status"
#define STATUS_PATH_MAX 64

int procmem_read(pid_t pid, procmem_snapshot_t *snap) {
    char path[STATUS_PATH_MAX];
    snprintf(path, sizeof(path), STATUS_PATH_FMT, (int)pid);

    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "procmem_read: cannot open %s: %s\n", path, strerror(errno));
        return -1;
    }

    snap->pid        = pid;
    snap->vm_rss_kb  = 0;
    snap->vm_virt_kb = 0;
    snap->vm_swap_kb = 0;

    char line[128];
    int found = 0;

    while (fgets(line, sizeof(line), f)) {
        uint64_t val;
        if (sscanf(line, "VmRSS: %" SCNu64 " kB", &val) == 1) {
            snap->vm_rss_kb = val;
            found++;
        } else if (sscanf(line, "VmSize: %" SCNu64 " kB", &val) == 1) {
            snap->vm_virt_kb = val;
            found++;
        } else if (sscanf(line, "VmSwap: %" SCNu64 " kB", &val) == 1) {
            snap->vm_swap_kb = val;
            found++;
        }
        if (found == 3) break;
    }

    fclose(f);

    if (found == 0) {
        fprintf(stderr, "procmem_read: no memory fields found for pid %d\n", (int)pid);
        return -1;
    }
    return 0;
}

void procmem_print(const procmem_snapshot_t *snap) {
    printf("PID %d | RSS: %" PRIu64 " kB | Virt: %" PRIu64 " kB | Swap: %" PRIu64 " kB\n",
           (int)snap->pid,
           snap->vm_rss_kb,
           snap->vm_virt_kb,
           snap->vm_swap_kb);
}
