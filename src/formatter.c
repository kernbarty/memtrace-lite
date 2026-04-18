#include "formatter.h"
#include <stdio.h>
#include <string.h>

void formatter_human_bytes(long kb, char *buf, size_t buf_len) {
    if (kb >= 1024 * 1024) {
        snprintf(buf, buf_len, "%.2f GB", kb / (1024.0 * 1024.0));
    } else if (kb >= 1024) {
        snprintf(buf, buf_len, "%.2f MB", kb / 1024.0);
    } else {
        snprintf(buf, buf_len, "%ld KB", kb);
    }
}

static int write_snapshot_plain(FILE *out, const MemSnapshot *snap) {
    char vm[32], rss[32], peak[32];
    formatter_human_bytes(snap->vm_rss_kb, rss, sizeof(rss));
    formatter_human_bytes(snap->vm_size_kb, vm, sizeof(vm));
    formatter_human_bytes(snap->vm_peak_kb, peak, sizeof(peak));
    return fprintf(out, "[t=%llds] VmRSS: %s | VmSize: %s | VmPeak: %s\n",
                   (long long)snap->timestamp, rss, vm, peak) < 0 ? -1 : 0;
}

static int write_snapshot_csv(FILE *out, const MemSnapshot *snap) {
    return fprintf(out, "%lld,%ld,%ld,%ld\n",
                   (long long)snap->timestamp,
                   snap->vm_rss_kb,
                   snap->vm_size_kb,
                   snap->vm_peak_kb) < 0 ? -1 : 0;
}

static int write_snapshot_json(FILE *out, const MemSnapshot *snap) {
    return fprintf(out,
                   "{\"timestamp\":%lld,\"vm_rss_kb\":%ld,"
                   "\"vm_size_kb\":%ld,\"vm_peak_kb\":%ld}",
                   (long long)snap->timestamp,
                   snap->vm_rss_kb,
                   snap->vm_size_kb,
                   snap->vm_peak_kb) < 0 ? -1 : 0;
}

int formatter_write_snapshot(FILE *out, const MemSnapshot *snap, OutputFormat fmt) {
    if (!out || !snap) return -1;
    switch (fmt) {
        case FORMAT_CSV:   return write_snapshot_csv(out, snap);
        case FORMAT_JSON:  return write_snapshot_json(out, snap);
        default:           return write_snapshot_plain(out, snap);
    }
}

int formatter_write_report(FILE *out, const MemTracker *tracker, OutputFormat fmt) {
    if (!out || !tracker) return -1;

    if (fmt == FORMAT_CSV) {
        fprintf(out, "timestamp,vm_rss_kb,vm_size_kb,vm_peak_kb\n");
    } else if (fmt == FORMAT_JSON) {
        fprintf(out, "[\n");
    }

    for (int i = 0; i < tracker->count; i++) {
        if (fmt == FORMAT_JSON && i > 0)
            fprintf(out, ",\n");
        if (formatter_write_snapshot(out, &tracker->snapshots[i], fmt) < 0)
            return -1;
    }

    if (fmt == FORMAT_JSON)
        fprintf(out, "\n]\n");

    return 0;
}
