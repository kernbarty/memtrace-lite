#ifndef QUOTA_H
#define QUOTA_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    QUOTA_OK = 0,
    QUOTA_WARN,
    QUOTA_EXCEEDED
} quota_status_t;

typedef struct {
    size_t soft_limit_kb;   /* warn threshold */
    size_t hard_limit_kb;   /* exceeded threshold */
    size_t current_kb;
    uint64_t exceed_count;
    uint64_t warn_count;
} quota_t;

void quota_init(quota_t *q, size_t soft_limit_kb, size_t hard_limit_kb);
void quota_reset(quota_t *q);
quota_status_t quota_check(quota_t *q, size_t current_kb);
quota_status_t quota_status(const quota_t *q);
const char *quota_status_str(quota_status_t status);
void quota_get_limits(const quota_t *q, size_t *soft_out, size_t *hard_out);

#endif /* QUOTA_H */
