#ifndef BUDGET_H
#define BUDGET_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    BUDGET_OK,
    BUDGET_WARN,
    BUDGET_EXCEEDED
} budget_status_t;

typedef struct {
    size_t limit_kb;
    size_t warn_threshold_kb;
    size_t current_kb;
    size_t peak_kb;
    unsigned int exceed_count;
} budget_t;

void budget_init(budget_t *b, size_t limit_kb, size_t warn_threshold_kb);
void budget_reset(budget_t *b);
budget_status_t budget_update(budget_t *b, size_t current_kb);
bool budget_is_exceeded(const budget_t *b);
float budget_usage_ratio(const budget_t *b);
void budget_get_summary(const budget_t *b, char *buf, size_t buf_len);

#endif
