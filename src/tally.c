#include "tally.h"

#include <string.h>
#include <stdio.h>

void tally_init(Tally *t)
{
    memset(t, 0, sizeof(*t));
}

int tally_register(Tally *t, const char *name)
{
    if (!name || t->num_categories >= TALLY_MAX_CATEGORIES)
        return -1;

    /* Reject duplicate names */
    for (int i = 0; i < t->num_categories; i++) {
        if (strncmp(t->categories[i].name, name, TALLY_NAME_LEN) == 0)
            return i;
    }

    int idx = t->num_categories++;
    strncpy(t->categories[idx].name, name, TALLY_NAME_LEN - 1);
    t->categories[idx].name[TALLY_NAME_LEN - 1] = '\0';
    t->categories[idx].count = 0;
    t->categories[idx].total = 0;
    return idx;
}

int tally_add(Tally *t, int cat_idx, uint64_t value)
{
    if (cat_idx < 0 || cat_idx >= t->num_categories)
        return -1;
    t->categories[cat_idx].count++;
    t->categories[cat_idx].total += value;
    return 0;
}

int tally_inc(Tally *t, int cat_idx)
{
    return tally_add(t, cat_idx, 0);
}

const TallyCategory *tally_get(const Tally *t, const char *name)
{
    if (!name)
        return NULL;
    for (int i = 0; i < t->num_categories; i++) {
        if (strncmp(t->categories[i].name, name, TALLY_NAME_LEN) == 0)
            return &t->categories[i];
    }
    return NULL;
}

void tally_reset(Tally *t)
{
    for (int i = 0; i < t->num_categories; i++) {
        t->categories[i].count = 0;
        t->categories[i].total = 0;
    }
    t->generation++;
}

int tally_format(const Tally *t, char *buf, size_t buf_len)
{
    int written = 0;
    int ret;

    ret = snprintf(buf + written, buf_len - (size_t)written,
                   "tally gen=%llu categories=%d\n",
                   (unsigned long long)t->generation,
                   t->num_categories);
    if (ret < 0 || (size_t)ret >= buf_len - (size_t)written)
        return written;
    written += ret;

    for (int i = 0; i < t->num_categories && (size_t)written < buf_len - 1; i++) {
        ret = snprintf(buf + written, buf_len - (size_t)written,
                       "  %-24s count=%-8llu total=%llu\n",
                       t->categories[i].name,
                       (unsigned long long)t->categories[i].count,
                       (unsigned long long)t->categories[i].total);
        if (ret < 0 || (size_t)ret >= buf_len - (size_t)written)
            break;
        written += ret;
    }
    return written;
}
