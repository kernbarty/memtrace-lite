#include "history_query.h"
#include <stddef.h>

bool history_query_stats(const History *h, HistoryStats *out) {
    if (!h || !out || h->count == 0) return false;
    MemSnapshot snap;
    long rss_sum = 0, vms_sum = 0;
    long rss_peak = 0, vms_peak = 0;
    for (size_t i = 0; i < h->count; i++) {
        history_get(h, i, &snap);
        rss_sum += snap.rss_kb;
        vms_sum += snap.vms_kb;
        if (snap.rss_kb > rss_peak) rss_peak = snap.rss_kb;
        if (snap.vms_kb > vms_peak) vms_peak = snap.vms_kb;
    }
    MemSnapshot first, last;
    history_get(h, 0, &first);
    history_latest(h, &last);
    out->rss_delta_kb = last.rss_kb - first.rss_kb;
    out->vms_delta_kb = last.vms_kb - first.vms_kb;
    out->rss_avg_kb   = (double)rss_sum / h->count;
    out->vms_avg_kb   = (double)vms_sum / h->count;
    out->rss_peak_kb  = rss_peak;
    out->vms_peak_kb  = vms_peak;
    out->sample_count = h->count;
    return true;
}

bool history_query_peak_rss(const History *h, MemSnapshot *out) {
    if (!h || !out || h->count == 0) return false;
    MemSnapshot snap, best;
    history_get(h, 0, &best);
    for (size_t i = 1; i < h->count; i++) {
        history_get(h, i, &snap);
        if (snap.rss_kb > best.rss_kb) best = snap;
    }
    *out = best;
    return true;
}

bool history_query_delta(const History *h, long *rss_delta_kb, long *vms_delta_kb) {
    if (!h || h->count < 2) return false;
    MemSnapshot first, last;
    history_get(h, 0, &first);
    history_latest(h, &last);
    if (rss_delta_kb) *rss_delta_kb = last.rss_kb - first.rss_kb;
    if (vms_delta_kb) *vms_delta_kb = last.vms_kb - first.vms_kb;
    return true;
}

bool history_query_find_rss_exceed(const History *h, long rss_threshold_kb, size_t *index_out) {
    if (!h || !index_out) return false;
    MemSnapshot snap;
    for (size_t i = 0; i < h->count; i++) {
        history_get(h, i, &snap);
        if (snap.rss_kb > rss_threshold_kb) {
            *index_out = i;
            return true;
        }
    }
    return false;
}
