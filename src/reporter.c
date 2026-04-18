#include "reporter.h"
#include <stdio.h>
#include <time.h>

void reporter_init(ReporterConfig *cfg) {
    if (!cfg) return;
    if (!cfg->output) {
        cfg->output = stdout;
    }
}

static void format_kb(char *buf, size_t bufsz, long kb) {
    if (kb >= 1024) {
        snprintf(buf, bufsz, "%.1f MB", kb / 1024.0);
    } else {
        snprintf(buf, bufsz, "%ld KB", kb);
    }
}

void reporter_print_snapshot(const ReporterConfig *cfg, const MemSnapshot *snap) {
    if (!cfg || !snap) return;

    char vm[32], rss[32], shared[32];
    format_kb(vm,     sizeof(vm),     snap->vm_size_kb);
    format_kb(rss,    sizeof(rss),    snap->vm_rss_kb);
    format_kb(shared, sizeof(shared), snap->vm_shared_kb);

    if (cfg->format == REPORT_FORMAT_CSV) {
        fprintf(cfg->output, "%ld,%ld,%ld,%ld\n",
                snap->timestamp_ms,
                snap->vm_size_kb,
                snap->vm_rss_kb,
                snap->vm_shared_kb);
    } else {
        fprintf(cfg->output,
                "[%ld ms] VmSize: %s | VmRSS: %s | Shared: %s\n",
                snap->timestamp_ms, vm, rss, shared);
    }
}

void reporter_print_delta(const ReporterConfig *cfg,
                          const MemSnapshot *prev,
                          const MemSnapshot *curr) {
    if (!cfg || !prev || !curr) return;

    long d_vm  = curr->vm_size_kb   - prev->vm_size_kb;
    long d_rss = curr->vm_rss_kb    - prev->vm_rss_kb;
    long d_shm = curr->vm_shared_kb - prev->vm_shared_kb;
    long d_t   = curr->timestamp_ms - prev->timestamp_ms;

    if (cfg->format == REPORT_FORMAT_CSV) {
        fprintf(cfg->output, "%ld,%ld,%ld,%ld\n",
                d_t, d_vm, d_rss, d_shm);
    } else {
        fprintf(cfg->output,
                "[+%ld ms] Delta => VmSize: %+ld KB | VmRSS: %+ld KB | Shared: %+ld KB\n",
                d_t, d_vm, d_rss, d_shm);
    }
}

void reporter_print_csv_header(const ReporterConfig *cfg) {
    if (!cfg) return;
    fprintf(cfg->output, "timestamp_ms,vm_size_kb,vm_rss_kb,vm_shared_kb\n");
}
