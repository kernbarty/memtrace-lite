#include "probe.h"
#include "procmem.h"
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>

static atomic_uint g_seq = 0;

static uint64_t read_kind(ProbeKind kind)
{
    ProcMemInfo info;
    if (procmem_read(0, &info) != 0)
        return 0;
    switch (kind) {
        case PROBE_KIND_RSS:   return (uint64_t)info.rss_kb;
        case PROBE_KIND_VSZ:   return (uint64_t)info.vsz_kb;
        case PROBE_KIND_HEAP:  return (uint64_t)info.heap_kb;
        default:               return (uint64_t)info.rss_kb;
    }
}

int probe_init(Probe *p, const char *name, const char *tag, ProbeKind kind)
{
    if (!p || !name) return -1;

    memset(p, 0, sizeof(*p));
    strncpy(p->name, name, PROBE_NAME_MAX - 1);
    if (tag)
        strncpy(p->tag, tag, PROBE_TAG_MAX - 1);

    p->kind        = kind;
    p->baseline_kb = read_kind(kind);
    p->value_kb    = p->baseline_kb;
    p->delta_kb    = 0;
    p->seq         = atomic_fetch_add(&g_seq, 1u);
    clock_gettime(CLOCK_MONOTONIC, &p->timestamp);
    return 0;
}

int probe_sample(Probe *p)
{
    if (!p) return -1;

    uint64_t prev = p->value_kb;
    p->value_kb   = read_kind(p->kind);
    p->delta_kb   = (int64_t)p->value_kb - (int64_t)p->baseline_kb;

    /* For DELTA kind, report change since last sample instead */
    if (p->kind == PROBE_KIND_DELTA)
        p->delta_kb = (int64_t)p->value_kb - (int64_t)prev;

    clock_gettime(CLOCK_MONOTONIC, &p->timestamp);
    return 0;
}

void probe_reset_baseline(Probe *p)
{
    if (!p) return;
    p->baseline_kb = p->value_kb;
    p->delta_kb    = 0;
}

long probe_age_ms(const Probe *p)
{
    if (!p) return -1;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long sec_diff  = (long)(now.tv_sec  - p->timestamp.tv_sec);
    long nsec_diff = (long)(now.tv_nsec - p->timestamp.tv_nsec);
    return sec_diff * 1000L + nsec_diff / 1000000L;
}

char *probe_format(const Probe *p, char *buf, size_t len)
{
    if (!p || !buf || len == 0) return buf;
    static const char *kind_str[] = { "rss", "vsz", "heap", "delta" };
    const char *ks = (p->kind <= PROBE_KIND_DELTA) ? kind_str[p->kind] : "?";
    snprintf(buf, len,
             "[probe #%u] %-32s tag=%-16s kind=%-5s val=%llu KB delta=%+lld KB",
             p->seq, p->name, p->tag[0] ? p->tag : "-",
             ks,
             (unsigned long long)p->value_kb,
             (long long)p->delta_kb);
    return buf;
}
