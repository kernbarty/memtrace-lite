#include "budget.h"
#include <stdio.h>
#include <string.h>

void budget_init(budget_t *b, size_t limit_kb, size_t warn_threshold_kb) {
    memset(b, 0, sizeof(*b));
    b->limit_kb = limit_kb;
    b->warn_threshold_kb = warn_threshold_kb;
}

void budget_reset(budget_t *b) {
    b->current_kb = 0;
    b->peak_kb = 0;
    b->exceed_count = 0;
}

budget_status_t budget_update(budget_t *b, size_t current_kb) {
    b->current_kb = current_kb;
    if (current_kb > b->peak_kb)
        b->peak_kb = current_kb;

    if (b->limit_kb > 0 && current_kb >= b->limit_kb) {
        b->exceed_count++;
        return BUDGET_EXCEEDED;
    }
    if (b->warn_threshold_kb > 0 && current_kb >= b->warn_threshold_kb)
        return BUDGET_WARN;
    return BUDGET_OK;
}

bool budget_is_exceeded(const budget_t *b) {
    return b->limit_kb > 0 && b->current_kb >= b->limit_kb;
}

float budget_usage_ratio(const budget_t *b) {
    if (b->limit_kb == 0) return 0.0f;
    return (float)b->current_kb / (float)b->limit_kb;
}

void budget_get_summary(const budget_t *b, char *buf, size_t buf_len) {
    snprintf(buf, buf_len,
        "budget: current=%zu KB, peak=%zu KB, limit=%zu KB, warn=%zu KB, "
        "ratio=%.2f, exceeded=%u times",
        b->current_kb, b->peak_kb, b->limit_kb,
        b->warn_threshold_kb, budget_usage_ratio(b), b->exceed_count);
}
