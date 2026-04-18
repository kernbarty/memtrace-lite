#include "filter.h"
#include <string.h>
#include <stdio.h>

void filter_init(MemFilter *f) {
    if (!f) return;
    memset(f, 0, sizeof(*f));
    f->flags      = FILTER_NONE;
    f->min_rss_kb = -1;
    f->max_rss_kb = -1;
    f->min_vsz_kb = -1;
    f->max_vsz_kb = -1;
}

void filter_set_rss(MemFilter *f, long min_kb, long max_kb) {
    if (!f) return;
    if (min_kb >= 0) { f->min_rss_kb = min_kb; f->flags |= FILTER_MIN_RSS; }
    if (max_kb >= 0) { f->max_rss_kb = max_kb; f->flags |= FILTER_MAX_RSS; }
}

void filter_set_vsz(MemFilter *f, long min_kb, long max_kb) {
    if (!f) return;
    if (min_kb >= 0) { f->min_vsz_kb = min_kb; f->flags |= FILTER_MIN_VSZ; }
    if (max_kb >= 0) { f->max_vsz_kb = max_kb; f->flags |= FILTER_MAX_VSZ; }
}

void filter_set_name(MemFilter *f, const char *substr) {
    if (!f || !substr) return;
    strncpy(f->name_substr, substr, sizeof(f->name_substr) - 1);
    f->name_substr[sizeof(f->name_substr) - 1] = '\0';
    f->flags |= FILTER_NAME_SUBSTR;
}

int filter_match(const MemFilter *f, const MemSnapshot *snap) {
    if (!f || !snap) return 0;
    if (f->flags == FILTER_NONE) return 1;

    if ((f->flags & FILTER_MIN_RSS) && snap->rss_kb < f->min_rss_kb)
        return 0;
    if ((f->flags & FILTER_MAX_RSS) && snap->rss_kb > f->max_rss_kb)
        return 0;
    if ((f->flags & FILTER_MIN_VSZ) && snap->vsz_kb < f->min_vsz_kb)
        return 0;
    if ((f->flags & FILTER_MAX_VSZ) && snap->vsz_kb > f->max_vsz_kb)
        return 0;
    if ((f->flags & FILTER_NAME_SUBSTR) &&
        strstr(snap->label, f->name_substr) == NULL)
        return 0;

    return 1;
}
