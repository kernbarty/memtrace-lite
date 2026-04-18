#include "filter_chain.h"
#include <string.h>

void filter_chain_init(FilterChain *chain) {
    if (!chain) return;
    memset(chain, 0, sizeof(*chain));
    chain->count = 0;
}

int filter_chain_add(FilterChain *chain, const MemFilter *f) {
    if (!chain || !f) return -1;
    if (chain->count >= FILTER_CHAIN_MAX) return -1;
    chain->filters[chain->count++] = *f;
    return 0;
}

int filter_chain_match(const FilterChain *chain, const MemSnapshot *snap) {
    if (!chain || !snap) return 0;
    for (int i = 0; i < chain->count; i++) {
        if (!filter_match(&chain->filters[i], snap))
            return 0;
    }
    return 1;
}

int filter_chain_apply(const FilterChain *chain,
                       const MemSnapshot *snaps, int n,
                       int *out, int out_max) {
    if (!chain || !snaps || !out) return 0;
    int found = 0;
    for (int i = 0; i < n && found < out_max; i++) {
        if (filter_chain_match(chain, &snaps[i]))
            out[found++] = i;
    }
    return found;
}
